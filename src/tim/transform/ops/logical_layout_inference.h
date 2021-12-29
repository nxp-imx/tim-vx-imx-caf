/****************************************************************************
 *
 *    Copyright (c) 2020 Vivante Corporation
 *
 *    Permission is hereby granted, free of charge, to any person obtaining a
 *    copy of this software and associated documentation files (the "Software"),
 *    to deal in the Software without restriction, including without limitation
 *    the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *    and/or sell copies of the Software, and to permit persons to whom the
 *    Software is furnished to do so, subject to the following conditions:
 *
 *    The above copyright notice and this permission notice shall be included in
 *    all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *    DEALINGS IN THE SOFTWARE.
 *
 *****************************************************************************/
#ifndef TIM_LAYOUT_INFER_LOGICAL_OPS_LAYOUT_INFERENCE_H_
#define TIM_LAYOUT_INFER_LOGICAL_OPS_LAYOUT_INFERENCE_H_

#include "ops/op_layout_inference.h"
#include "direct_map_op_impl.h"
#include "tim/vx/ops/logical.h"

namespace tim {
namespace transform {
template <typename OpTpye>
class LogicalOpsLayoutInfer : public OpLayoutInfer {
 public:
  LogicalOpsLayoutInfer(
      const std::shared_ptr<vx::Operation> op,
      std::shared_ptr<layout_inference_impl::LayoutInferContext>& context)
      : OpLayoutInfer(op, context) {}

  void OnInputs(
      std::vector<std::shared_ptr<vx::Tensor>>& next_tensors) override {
    auto required_pv = AlignPermuteVectorForMutilInputs();
    auto infer_out = CreateOutputsTensor(required_pv);
    auto logical_op = context_->infer_graph_->CreateOperation<OpTpye>();
    for (const auto& i_src : op_->impl()->InputsTensor()) {
      (*logical_op).BindInput(context_->GetMapedTensor(i_src));
    }
    (*logical_op).BindOutput(infer_out[0]);

    context_->SetPermuteVector(op_->impl()->OutputsTensor()[0], required_pv);
    next_tensors.push_back(op_->impl()->OutputsTensor()[0]);
  }
};

using LogicalAndLayoutInfer = LogicalOpsLayoutInfer<vx::ops::LogicalAnd>;
using LogicalOrLayoutInfer = LogicalOpsLayoutInfer<vx::ops::LogicalOr>;

}  // namespace transform
}  // namespace tim
#endif