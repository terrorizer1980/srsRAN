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

#ifndef SRSRAN_PHY_INTERFACE_TYPES_H
#define SRSRAN_PHY_INTERFACE_TYPES_H

#include "srsran/srsran.h"

/// Common types defined by the PHY layer.

inline bool operator==(const srsran_tdd_config_t& a, const srsran_tdd_config_t& b)
{
  return (a.sf_config == b.sf_config && a.ss_config == b.ss_config && a.configured == b.configured);
}

inline bool operator!=(const srsran_tdd_config_t& a, const srsran_tdd_config_t& b)
{
  return !(a == b);
}

inline bool operator==(const srsran_prach_cfg_t& a, const srsran_prach_cfg_t& b)
{
  return (a.config_idx == b.config_idx && a.root_seq_idx == b.root_seq_idx && a.zero_corr_zone == b.zero_corr_zone &&
          a.freq_offset == b.freq_offset && a.num_ra_preambles == b.num_ra_preambles && a.hs_flag == b.hs_flag &&
          a.tdd_config == b.tdd_config && a.enable_successive_cancellation == b.enable_successive_cancellation &&
          a.enable_freq_domain_offset_calc == b.enable_freq_domain_offset_calc);
}

inline bool operator!=(const srsran_prach_cfg_t& a, const srsran_prach_cfg_t& b)
{
  return !(a == b);
}

namespace srsue {

struct phy_meas_nr_t {
  float    rsrp;
  float    rsrq;
  float    sinr;
  float    cfo_hz;
  uint32_t arfcn_nr;
  uint32_t pci_nr;
};

struct phy_meas_t {
  float    rsrp;
  float    rsrq;
  float    cfo_hz;
  uint32_t earfcn;
  uint32_t pci;
};

struct phy_cell_t {
  uint32_t pci;
  uint32_t earfcn;
  float    cfo_hz;
};

} // namespace srsue

#endif // SRSRAN_PHY_INTERFACE_TYPES_H