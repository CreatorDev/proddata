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


#include "vector_operations.h"
#include <vector>
#include <stdexcept>

namespace vector_operations {

  void replace(std::vector<uint8_t> *data, const std::vector<uint8_t> &replacement_vector,
                      int position) {
    auto erase_end = data->begin() + position + replacement_vector.size();

    if (erase_end > data->end()) {
      throw std::runtime_error("Cannot replace content of vector with replacement vector");
    }

    data->erase(data->begin() + position, erase_end);
    data->insert(data->begin() + position, replacement_vector.begin(), replacement_vector.end());
  }
}  // namespace vector_operations
