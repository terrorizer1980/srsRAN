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

#ifndef SRSRAN_UE_DL_NR_H
#define SRSRAN_UE_DL_NR_H

#include "srsran/phy/ch_estimation/dmrs_pdcch.h"
#include "srsran/phy/common/phy_common_nr.h"
#include "srsran/phy/dft/ofdm.h"
#include "srsran/phy/phch/dci_nr.h"
#include "srsran/phy/phch/pdcch_nr.h"
#include "srsran/phy/phch/pdsch_nr.h"
#include "srsran/phy/phch/uci_cfg_nr.h"

/**
 * Maximum number of CORESET
 * @remark Defined in TS 38.331 by maxNrofControlResourceSets-1
 */
#define SRSRAN_UE_DL_NR_MAX_NOF_CORESET 12

/**
 * Maximum number of Search spaces
 * @remark Defined in TS 38.331 by maxNrofSearchSpaces-1
 */
#define SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE 40

/**
 * Maximum number of DCI messages to receive
 */
#define SRSRAN_MAX_DCI_MSG_NR 4

typedef struct SRSRAN_API {
  srsran_pdsch_nr_args_t pdsch;
  srsran_pdcch_nr_args_t pdcch;
  uint32_t               nof_rx_antennas;
  uint32_t               nof_max_prb;
  float                  pdcch_dmrs_corr_thr;
  float                  pdcch_dmrs_epre_thr;
} srsran_ue_dl_nr_args_t;

typedef struct SRSRAN_API {
  srsran_coreset_t coreset[SRSRAN_UE_DL_NR_MAX_NOF_CORESET]; ///< PDCCH Control resource sets (CORESET) collection
  bool             coreset_present[SRSRAN_UE_DL_NR_MAX_NOF_CORESET]; ///< CORESET present flags

  srsran_search_space_t search_space[SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE];
  bool                  search_space_present[SRSRAN_UE_DL_NR_MAX_NOF_SEARCH_SPACE];

  uint16_t              ra_rnti; ///< Needs to be deduced from the PRACH configuration
  srsran_search_space_t ra_search_space;
  bool                  ra_search_space_present;
} srsran_ue_dl_nr_pdcch_cfg_t;

typedef struct {
  uint32_t scell_idx;  ///< Serving cell index
  uint32_t v_dai_dl;   ///< Downlink Assigment Index
  bool dci_format_1_1; ///< Set to true if the PDSCH transmission is triggered by a PDCCH DCI format 1_1 transmission
  uint32_t k1;         ///< HARQ feedback timing
  uint16_t rnti;
  uint32_t pucch_resource_id;
} srsran_pdsch_ack_resource_nr_t;

typedef struct {
  srsran_pdsch_ack_resource_nr_t resource;
  uint8_t                        value[SRSRAN_MAX_CODEWORDS]; // 0/1 or 2 for DTX
  bool present; // set to true if there is a PDSCH on serving cell c associated with PDCCH in PDCCH monitoring occasion
                // m, or there is a PDCCH indicating SPS PDSCH release on serving cell c
  bool dl_bwp_changed; // set to true if PDCCH monitoring occasion m is before an active DL BWP change on serving cell c
  bool ul_bwp_changed; // set to true if an active UL BWP change on the PCell and an active DL BWP change is not
                       // triggered by a DCI format 1_1 in PDCCH monitoring occasion m
  bool second_tb_present; // set to true if two TB were detected in the PDCCH monitoring occasion m
} srsran_pdsch_ack_m_nr_t;

#define SRSRAN_UCI_NR_MAX_M 10

typedef struct {
  uint32_t                M;
  srsran_pdsch_ack_m_nr_t m[SRSRAN_UCI_NR_MAX_M];
} srsran_pdsch_ack_cc_nr_t;

typedef struct {
  srsran_pdsch_ack_cc_nr_t cc[SRSRAN_MAX_CARRIERS];
  uint32_t                 nof_cc;
  bool                     use_pusch; // Set to true, if UCI bits are carried by PUSCH
} srsran_pdsch_ack_nr_t;

typedef struct SRSRAN_API {
  bool harq_ack_spatial_bundling_pucch; ///< Param harq-ACK-SpatialBundlingPUCCH, set to true if provided
  bool harq_ack_spatial_bundling_pusch; ///< Param harq-ACK-SpatialBundlingPUSCH, set to true if provided
  srsran_pdsch_harq_ack_codebook_t pdsch_harq_ack_codebook; ///< pdsch-HARQ-ACK-Codebook configuration
  bool max_cw_sched_dci_is_2; ///< Param maxNrofCodeWordsScheduledByDCI, set to true if present and equal to 2

  uint32_t dl_data_to_ul_ack[SRSRAN_MAX_NOF_DL_DATA_TO_UL];
  uint32_t nof_dl_data_to_ul_ack;
} srsran_ue_dl_nr_harq_ack_cfg_t;

typedef struct SRSRAN_API {
  uint32_t                    coreset_id;
  uint32_t                    ss_id;
  srsran_dci_location_t       location;
  srsran_dmrs_pdcch_measure_t measure;
  srsran_pdcch_nr_res_t       result;
} srsran_ue_dl_nr_pdcch_info_t;

typedef struct SRSRAN_API {
  uint32_t max_prb;
  uint32_t nof_rx_antennas;
  float    pdcch_dmrs_corr_thr;
  float    pdcch_dmrs_epre_thr;

  srsran_carrier_nr_t         carrier;
  srsran_ue_dl_nr_pdcch_cfg_t cfg;

  srsran_ofdm_t fft[SRSRAN_MAX_PORTS];

  cf_t*                 sf_symbols[SRSRAN_MAX_PORTS];
  srsran_chest_dl_res_t chest;
  srsran_pdsch_nr_t     pdsch;
  srsran_dmrs_sch_t     dmrs_pdsch;

  srsran_dmrs_pdcch_estimator_t dmrs_pdcch[SRSRAN_UE_DL_NR_MAX_NOF_CORESET];
  srsran_pdcch_nr_t             pdcch;
  srsran_dmrs_pdcch_ce_t*       pdcch_ce;

  /// Store Blind-search information from all possible candidate locations for debug purposes
  srsran_ue_dl_nr_pdcch_info_t pdcch_info[SRSRAN_MAX_NOF_CANDIDATES_SLOT_NR];
  uint32_t                     pdcch_info_count;

  /// Temporally stores Found DCI messages from all SS
  srsran_dci_msg_nr_t dci_msg[SRSRAN_MAX_DCI_MSG_NR];
  uint32_t            dci_msg_count;

  srsran_dci_msg_nr_t pending_ul_dci_msg[SRSRAN_MAX_DCI_MSG_NR];
  uint32_t            pending_ul_dci_count;
} srsran_ue_dl_nr_t;

SRSRAN_API int
srsran_ue_dl_nr_init(srsran_ue_dl_nr_t* q, cf_t* input[SRSRAN_MAX_PORTS], const srsran_ue_dl_nr_args_t* args);

SRSRAN_API int srsran_ue_dl_nr_set_carrier(srsran_ue_dl_nr_t* q, const srsran_carrier_nr_t* carrier);

SRSRAN_API int srsran_ue_dl_nr_set_pdcch_config(srsran_ue_dl_nr_t* q, const srsran_ue_dl_nr_pdcch_cfg_t* cfg);

SRSRAN_API void srsran_ue_dl_nr_free(srsran_ue_dl_nr_t* q);

SRSRAN_API void srsran_ue_dl_nr_estimate_fft(srsran_ue_dl_nr_t* q, const srsran_slot_cfg_t* slot_cfg);

SRSRAN_API int srsran_ue_dl_nr_find_dl_dci(srsran_ue_dl_nr_t*       q,
                                           const srsran_slot_cfg_t* slot_cfg,
                                           uint16_t                 rnti,
                                           srsran_rnti_type_t       rnti_type,
                                           srsran_dci_dl_nr_t*      dci_dl_list,
                                           uint32_t                 nof_dci_msg);

SRSRAN_API int srsran_ue_dl_nr_find_ul_dci(srsran_ue_dl_nr_t*       q,
                                           const srsran_slot_cfg_t* slot_cfg,
                                           uint16_t                 rnti,
                                           srsran_rnti_type_t       rnti_type,
                                           srsran_dci_ul_nr_t*      dci_ul_list,
                                           uint32_t                 nof_dci_msg);

SRSRAN_API int srsran_ue_dl_nr_decode_pdsch(srsran_ue_dl_nr_t*         q,
                                            const srsran_slot_cfg_t*   slot,
                                            const srsran_sch_cfg_nr_t* cfg,
                                            srsran_pdsch_res_nr_t*     res);

SRSRAN_API int srsran_ue_dl_nr_pdsch_info(const srsran_ue_dl_nr_t*    q,
                                          const srsran_sch_cfg_nr_t*  cfg,
                                          const srsran_pdsch_res_nr_t res[SRSRAN_MAX_CODEWORDS],
                                          char*                       str,
                                          uint32_t                    str_len);

SRSRAN_API int srsran_ue_dl_nr_pdsch_ack_resource(const srsran_ue_dl_nr_harq_ack_cfg_t* cfg,
                                                  const srsran_dci_dl_nr_t*             dci_dl,
                                                  srsran_pdsch_ack_resource_nr_t*       pdsch_ack_resource);

SRSRAN_API int srsran_ue_dl_nr_gen_ack(const srsran_ue_dl_nr_harq_ack_cfg_t* cfg,
                                       const srsran_pdsch_ack_nr_t*          ack_info,
                                       srsran_uci_data_nr_t*                 uci_data);

#endif // SRSRAN_UE_DL_NR_H
