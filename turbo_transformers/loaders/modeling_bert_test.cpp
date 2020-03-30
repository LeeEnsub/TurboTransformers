// Copyright 2020 Tencent
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "turbo_transformers/loaders/modeling_bert.h"

#include <cmath>

#include "catch2/catch.hpp"
#include "turbo_transformers/core/macros.h"

namespace turbo_transformers {
namespace loaders {

bool CheckCppBert(bool use_cuda, bool only_input) {
  BertModel model("models/bert.npz",
                  use_cuda ? DLDeviceType::kDLGPU : DLDeviceType::kDLCPU, 12,
                  12);
  std::vector<std::vector<int64_t> > position_ids{{1, 0, 0, 0}, {1, 1, 1, 0}};
  std::vector<std::vector<int64_t> > segment_ids{{1, 1, 1, 0}, {1, 0, 0, 0}};
  if (only_input) {
    position_ids.clear();
    segment_ids.clear();
  }
  auto vec = model({{12166, 10699, 16752, 4454}, {5342, 16471, 817, 16022}},
                   position_ids, segment_ids, PoolingType::kFirst);
  REQUIRE(vec.size() == 768 * 2);
  // Write a better UT
  for (size_t i = 0; i < vec.size(); ++i) {
    REQUIRE(!std::isnan(vec.data()[i]));
    REQUIRE(!std::isinf(vec.data()[i]));
  }
  if (only_input) {
    REQUIRE(fabs(vec.data()[0] - -0.9791) < 1e-3);
    REQUIRE(fabs(vec.data()[1] - 0.8283) < 1e-3);
    REQUIRE(fabs(vec.data()[768] - 0.3837) < 1e-3);
    REQUIRE(fabs(vec.data()[768 + 1] - 0.1659) < 1e-3);
  } else {
    REQUIRE(fabs(vec.data()[0] - -0.3760) < 1e-3);
    REQUIRE(fabs(vec.data()[1] - 0.5674) < 1e-3);
    REQUIRE(fabs(vec.data()[768] - -0.2701) < 1e-3);
    REQUIRE(fabs(vec.data()[768 + 1] - 0.2676) < 1e-3);
  }
  return true;
}

TEST_CASE("Bert", "Cpp interface") {
  CheckCppBert(false /*use_cuda*/, true /* only_input*/);
  CheckCppBert(false /*use_cuda*/, false /* only_input*/);
  if (core::IsCompiledWithCUDA()) {
    CheckCppBert(true /*use_cuda*/, true /* only_input*/);
    CheckCppBert(true /*use_cuda*/, false /* only_input*/);
  }
}
}  // namespace loaders
}  // namespace turbo_transformers