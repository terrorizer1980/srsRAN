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

/******************************************************************************
 *  File:         tti_synch_cv.h
 *  Description:  Implements tti_sync interface with condition variables.
 *  Reference:
 *****************************************************************************/

#ifndef SRSRAN_TTI_SYNC_CV_H
#define SRSRAN_TTI_SYNC_CV_H

#include "srsran/common/tti_sync.h"
#include <pthread.h>

namespace srsran {

class tti_sync_cv : public tti_sync
{
public:
  tti_sync_cv(uint32_t modulus = 10240);
  ~tti_sync_cv();
  void     increase();
  void     increase(uint32_t cnt);
  uint32_t wait();
  void     resync();
  void     set_producer_cntr(uint32_t producer_cntr);

private:
  pthread_cond_t  cond;
  pthread_mutex_t mutex;
};

} // namespace srsran

#endif // SRSRAN_TTI_SYNC_CV_H