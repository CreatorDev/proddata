/**
 * @file
 * vector operations
 *
 * @author Imagination Technologies
 *
 * @copyright <b>Copyright 2016 by Imagination Technologies Limited and/or its affiliated group companies.</b>
 *      All rights reserved.  No part of this software, either
 *      material or conceptual may be copied or distributed,
 *      transmitted, transcribed, stored in a retrieval system
 *      or translated into any human or computer language in any
 *      form by any means, electronic, mechanical, manual or
 *      other-wise, or disclosed to the third parties without the
 *      express written permission of Imagination Technologies
 *      Limited, Home Park Estate, Kings Langley, Hertfordshire,
 *      WD4 8LZ, U.K.
 */

#ifndef VECTOROPERATIONS_H_
#define VECTOROPERATIONS_H_

#include <cstdint>
#include <vector>

namespace vector_operations {

  /**
   * Replace vector content with replacement vector from specified position
   *
   * @param[out] data vector whose content is to be replaced
   * @param[in] replacement_vector replacement vector
   * @param[in] position position in the vector from where content is to be replaced
   */
  void replace(std::vector<uint8_t> *data, const std::vector<uint8_t> &replacement_vector,
               int position);

}  // namespace vector_operations

#endif  // VECTOROPERATIONS_H_
