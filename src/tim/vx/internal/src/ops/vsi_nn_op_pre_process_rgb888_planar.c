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


#include <string.h>
#include <stdlib.h>

#include "vsi_nn_types.h"
#include "vsi_nn_log.h"
#include "vsi_nn_node.h"
#include "vsi_nn_prv.h"
#include "vsi_nn_ops.h"
#include "vsi_nn_tensor.h"
#include "utils/vsi_nn_util.h"
#include "kernel/vsi_nn_kernel.h"
#include "utils/vsi_nn_constraint_check.h"

typedef struct _pre_process_rgb888_planar_local_data_t {
    int32_t scale_x;
    int32_t scale_y;
    vsi_bool enable_copy;
    vsi_bool enable_perm;
} pre_process_rgb888_planar_local_data_t;

/*
 Declare number of input and output.
 */
#define _INPUT_NUM          (3)
#define _OUTPUT_NUM         (3)

static vsi_status op_compute
    (
    vsi_nn_node_t * self,
    vsi_nn_tensor_t ** inputs,
    vsi_nn_tensor_t ** outputs
    )
{
    vsi_status status = VSI_FAILURE;
    vsi_nn_kernel_param_t * param = NULL;
    vsi_nn_kernel_node_t    n = NULL;
    param = vsi_nn_kernel_param_create();

    vsi_nn_kernel_param_add_int32( param, "scale_x", self->nn_param.pre_process_rgb888_planar.local->scale_x );
    vsi_nn_kernel_param_add_int32( param, "scale_y", self->nn_param.pre_process_rgb888_planar.local->scale_y );
    vsi_nn_kernel_param_add_int32( param, "left", self->nn_param.pre_process_rgb888_planar.rect.left );
    vsi_nn_kernel_param_add_int32( param, "top", self->nn_param.pre_process_rgb888_planar.rect.top );
    vsi_nn_kernel_param_add_int32( param, "width", self->nn_param.pre_process_rgb888_planar.rect.width );
    vsi_nn_kernel_param_add_int32( param, "height", self->nn_param.pre_process_rgb888_planar.rect.height );
    vsi_nn_kernel_param_add_float32( param, "r_mean", self->nn_param.pre_process_rgb888_planar.r_mean );
    vsi_nn_kernel_param_add_float32( param, "g_mean", self->nn_param.pre_process_rgb888_planar.g_mean );
    vsi_nn_kernel_param_add_float32( param, "b_mean", self->nn_param.pre_process_rgb888_planar.b_mean );
    vsi_nn_kernel_param_add_float32( param, "scale", self->nn_param.pre_process_rgb888_planar.scale );
    vsi_nn_kernel_param_add_int32( param, "enable_copy", self->nn_param.pre_process_rgb888_planar.local->enable_copy );

    n = vsi_nn_kernel_selector( self->graph, "pre_process_rgb888_planar", inputs, 3, outputs, 3, param );
    if ( n != NULL )
    {
        self->n = (vx_node)n;
        status = VSI_SUCCESS;
    }

    if (param != NULL)
    {
        vsi_nn_kernel_param_release( &param );
    }

    return status;
} /* op_compute() */

static vsi_bool op_check
    (
    vsi_nn_node_t * self,
    vsi_nn_tensor_t ** inputs,
    vsi_nn_tensor_t ** outputs
    )
{
    if (inputs[1] == NULL)
    {
        BEGIN_IO_TYPE_DECL(PRE_PROCESS_RGB888_PLANAR, 1, 3)
            IO_TYPE(D_U8, D_U8|Q_ASYM, D_U8|Q_ASYM, D_U8|Q_ASYM)
            IO_TYPE(D_U8, D_I8|Q_DFP,  D_I8|Q_DFP,  D_I8|Q_DFP)
            IO_TYPE(D_U8, D_I16|Q_DFP, D_I16|Q_DFP, D_I16|Q_DFP)
            IO_TYPE(D_U8, D_F16,       D_F16,       D_F16)
        END_IO_TYPE_DECL(PRE_PROCESS_RGB888_PLANAR)

        if (!VALIDATE_OP_IO_TYPES(PRE_PROCESS_RGB888_PLANAR, self, inputs, 1,
            outputs, self->output.num)) {
            char* desc = generate_op_io_types_desc(inputs,
                    self->input.num, outputs, self->output.num);
            VSILOGE("Inputs/Outputs data type not support: %s", desc);
            destroy_op_io_types_desc(desc);
            return FALSE;
        }
    }
    else
    {
        BEGIN_IO_TYPE_DECL(PRE_PROCESS_RGB888_PLANAR, 3, 3)
            IO_TYPE(D_U8, D_U8, D_U8, D_U8|Q_ASYM, D_U8|Q_ASYM, D_U8|Q_ASYM)
            IO_TYPE(D_U8, D_U8, D_U8, D_I8|Q_DFP,  D_I8|Q_DFP,  D_I8|Q_DFP)
            IO_TYPE(D_U8, D_U8, D_U8, D_I16|Q_DFP, D_I16|Q_DFP, D_I16|Q_DFP)
            IO_TYPE(D_U8, D_U8, D_U8, D_F16,       D_F16,       D_F16)
        END_IO_TYPE_DECL(PRE_PROCESS_RGB888_PLANAR)

        if (!VALIDATE_OP_IO_TYPES(PRE_PROCESS_RGB888_PLANAR, self, inputs, self->input.num,
            outputs, self->output.num)) {
            char* desc = generate_op_io_types_desc(inputs,
                    self->input.num, outputs, self->output.num);
            VSILOGE("Inputs/Outputs data type not support: %s", desc);
            destroy_op_io_types_desc(desc);
            return FALSE;
        }
    }

    return TRUE;
} /* op_check() */

static vsi_bool op_setup
    (
    vsi_nn_node_t * self,
    vsi_nn_tensor_t ** inputs,
    vsi_nn_tensor_t ** outputs
    )
{
    vsi_nn_pre_process_rgb888_planar_param * p = NULL;
    uint32_t i = 0, j = 0;
    p = (vsi_nn_pre_process_rgb888_planar_param *)&(self->nn_param.pre_process_rgb888_planar);

    if (p->rect.width == 0 || p->rect.height == 0)
    {
        VSILOGE("Image size cannot be zero !(PRE_PROCESS_RGB888_PLANAR)\n");
        return FALSE;
    }
    else
    {
        for (i = 0; i < p->output_attr.dim_num; i++)
        {
            if (p->output_attr.size[i] == 0)
            {
                VSILOGE("output size cannot be zero!(PRE_PROCESS_RGB888_PLANAR)\n");
                return FALSE;
            }
        }
    }

    for (j = 0; j < 3; j++)
    {
        if ( VSI_NN_DIM_AUTO == outputs[j]->attr.dim_num )
        {
            if (p->output_attr.dim_num > 0)
            {
                outputs[j]->attr.dim_num = p->output_attr.dim_num;
                for (i = 0; i < p->output_attr.dim_num; i++)
                {
                    outputs[j]->attr.dim_num = p->output_attr.dim_num;
                    outputs[j]->attr.size[i] = p->output_attr.size[i];
                }
            }
            else
            {
                VSILOGE("output dim num cannot be zero!(PRE_PROCESS_RGB888_PLANAR)\n");
                return FALSE;
            }
        }
    }

    p->local->scale_x = (int32_t)((p->rect.width << 15) / outputs[0]->attr.size[0]);
    p->local->scale_y = (int32_t)((p->rect.height << 15) / outputs[0]->attr.size[1]);

    p->local->enable_copy = ((p->local->scale_x == p->local->scale_y) && (p->local->scale_x == (1 << 15)));

    return TRUE;
} /* op_setup() */

static vsi_status op_init
    (
    vsi_nn_node_t* self
    )
{
    self->nn_param.pre_process_rgb888_planar.local   =
    (pre_process_rgb888_planar_local_data_t *)malloc(sizeof(pre_process_rgb888_planar_local_data_t));

    if (NULL == self->nn_param.pre_process_rgb888_planar.local)
    {
        return  VX_ERROR_NO_MEMORY;
    }
    memset(self->nn_param.pre_process_rgb888_planar.local, 0, sizeof(pre_process_rgb888_planar_local_data_t));

    return VSI_SUCCESS;
} /* op_init() */

static vsi_status op_deinit
    (
    vsi_nn_node_t* self
    )
{
    vsi_status status = VSI_SUCCESS;

    vsi_nn_safe_free(self->nn_param.pre_process_rgb888_planar.local);
    vsi_nn_op_common_deinit(self);

    return status;
} /* op_deinit() */

__BEGIN_DECLS

/* Registrar */
DEF_OP_REG
    (
    /* op_name    */ PRE_PROCESS_RGB888_PLANAR,
    /* init       */ op_init,
    /* compute    */ op_compute,
    /* deinit     */ op_deinit,
    /* check      */ op_check,
    /* setup      */ op_setup,
    /* optimize   */ NULL,
    /* input_num  */ _INPUT_NUM,
    /* output_num */ _OUTPUT_NUM
    );

__END_DECLS
