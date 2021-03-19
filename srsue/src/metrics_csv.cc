/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsue/hdr/metrics_csv.h"

#include <float.h>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sstream>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>

using namespace std;

namespace srsue {

metrics_csv::metrics_csv(std::string filename, bool append_)
{
  std::ios_base::openmode flags = std::ios_base::out;
  if (append_) {
    // check if file exists
    ifstream f(filename.c_str());
    if (f.good()) {
      file_exists = true;
      flags |= std::ios_base::app;
    }
  }
  file.open(filename.c_str(), flags);
}

metrics_csv::~metrics_csv()
{
  stop();
}

void metrics_csv::set_ue_handle(ue_metrics_interface* ue_)
{
  ue = ue_;
}

void metrics_csv::set_flush_period(const uint32_t flush_period_sec_)
{
  flush_period_sec = flush_period_sec_;
}

void metrics_csv::stop()
{
  std::unique_lock<std::mutex> lock(mutex);
  if (file.is_open()) {
    file << "#eof\n";
    file.flush();
    file.close();
  }
}

void metrics_csv::set_metrics(const ue_metrics_t& metrics, const uint32_t period_usec)
{
  std::unique_lock<std::mutex> lock(mutex);

  time_ms += period_usec / 1000;

  if (file.is_open() && ue != NULL) {
    if (n_reports == 0 && !file_exists) {
      file << "time;cc;earfcn;pci;rsrp;pl;cfo;pci_neigh;rsrp_neigh;cfo_neigh;dl_mcs;dl_snr;dl_turbo;dl_brate;dl_bler;"
              "ul_ta;distance_km;speed_kmph;ul_mcs;ul_buff;ul_brate;ul_"
              "bler;"
              "rf_o;rf_"
              "u;rf_l;is_attached;"
              "proc_rmem;proc_rmem_kB;proc_vmem;proc_vmem_kB;sys_mem;proc_cpu;thread_count\n";
    }

    for (uint32_t r = 0; r < metrics.phy.nof_active_cc; r++) {
      file << time_ms << ";";

      // CC and PCI
      file << r << ";";
      file << metrics.phy.info[r].dl_earfcn << ";";
      file << metrics.phy.info[r].pci << ";";

      // Print PHY metrics for first CC
      file << float_to_string(metrics.phy.ch[r].rsrp, 2);
      file << float_to_string(metrics.phy.ch[r].pathloss, 2);
      file << float_to_string(metrics.phy.sync[r].cfo, 2);

      // Find strongest neighbour for this EARFCN (cells are ordered)
      bool has_neighbour = false;
      for (auto& c : metrics.stack.rrc.neighbour_cells) {
        if (c.earfcn == metrics.phy.info[r].dl_earfcn && c.pci != metrics.phy.info[r].pci) {
          file << c.pci << ";";
          file << float_to_string(c.rsrp, 2);
          file << float_to_string(c.cfo_hz, 2);
          has_neighbour = true;
          break;
        }
      }
      if (!has_neighbour) {
        file << "n/a;";
        file << "n/a;";
        file << "n/a;";
      }

      file << float_to_string(metrics.phy.dl[r].mcs, 2);
      file << float_to_string(metrics.phy.ch[r].sinr, 2);
      file << float_to_string(metrics.phy.dl[r].turbo_iters, 2);

      if (metrics.stack.mac[r].rx_brate > 0) {
        file << float_to_string(metrics.stack.mac[r].rx_brate / (metrics.stack.mac[r].nof_tti * 1e-3), 2);
      } else {
        file << float_to_string(0, 2);
      }

      int rx_pkts   = metrics.stack.mac[r].rx_pkts;
      int rx_errors = metrics.stack.mac[r].rx_errors;
      if (rx_pkts > 0) {
        file << float_to_string((float)100 * rx_errors / rx_pkts, 1);
      } else {
        file << float_to_string(0, 2);
      }

      file << float_to_string(metrics.phy.sync[r].ta_us, 2);
      file << float_to_string(metrics.phy.sync[r].distance_km, 2);
      file << float_to_string(metrics.phy.sync[r].speed_kmph, 2);
      file << float_to_string(metrics.phy.ul[r].mcs, 2);
      file << float_to_string((float)metrics.stack.mac[r].ul_buffer, 2);

      if (metrics.stack.mac[r].tx_brate > 0) {
        file << float_to_string(metrics.stack.mac[r].tx_brate / (metrics.stack.mac[r].nof_tti * 1e-3), 2);
      } else {
        file << float_to_string(0, 2);
      }

      // Sum UL BLER for all CCs
      int tx_pkts   = metrics.stack.mac[r].tx_pkts;
      int tx_errors = metrics.stack.mac[r].tx_errors;
      if (tx_pkts > 0) {
        file << float_to_string((float)100 * tx_errors / tx_pkts, 1);
      } else {
        file << float_to_string(0, 2);
      }

      file << float_to_string(metrics.rf.rf_o, 2);
      file << float_to_string(metrics.rf.rf_u, 2);
      file << float_to_string(metrics.rf.rf_l, 2);
      file << (metrics.stack.rrc.state == RRC_STATE_CONNECTED ? "1.0" : "0.0") << ";";

      // Write system metrics.
      const srsran::sys_metrics_t& m = metrics.sys;
      file << float_to_string(m.process_realmem, 2);
      file << std::to_string(m.process_realmem_kB) << ";";
      file << float_to_string(m.process_virtualmem, 2);
      file << std::to_string(m.process_virtualmem_kB) << ";";
      file << float_to_string(m.system_mem, 2);
      file << float_to_string(m.process_cpu_usage, 2);
      file << std::to_string(m.thread_count);

      file << "\n";
    }

    n_reports++;

    if (flush_period_sec > 0) {
      flush_time_ms += period_usec / 1000;
      if (flush_time_ms / 1000 >= flush_period_sec) {
        file.flush();
        flush_time_ms -= flush_period_sec * 1000;
      }
    }
  } else {
    std::cout << "couldn't write CSV file." << std::endl;
  }
}

std::string metrics_csv::float_to_string(float f, int digits, bool add_semicolon)
{
  std::ostringstream os;
  const int          precision = (f == 0.0) ? digits - 1 : digits - log10f(fabs(f)) - 2 * DBL_EPSILON;
  os << std::fixed << std::setprecision(precision) << f;
  if (add_semicolon)
    os << ';';
  return os.str();
}

} // namespace srsue
