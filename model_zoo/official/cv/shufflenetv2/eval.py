# Copyright 2020 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ============================================================================
"""evaluate_imagenet"""
import argparse

import mindspore.nn as nn
from mindspore import context
from mindspore.train.model import Model
from mindspore.train.serialization import load_checkpoint, load_param_into_net

from src.config import config_gpu as cfg
from src.dataset import create_dataset
from src.shufflenetv2 import ShuffleNetV2
from src.CrossEntropySmooth import CrossEntropySmooth

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='image classification evaluation')
    parser.add_argument('--checkpoint', type=str, default='', help='checkpoint of ShuffleNetV2 (Default: None)')
    parser.add_argument('--dataset_path', type=str, default='', help='Dataset path')
    parser.add_argument('--platform', type=str, default='GPU', choices=('Ascend', 'GPU'), help='run platform')
    args_opt = parser.parse_args()

    if args_opt.platform != 'GPU':
        raise ValueError("Only supported GPU training.")

    context.set_context(mode=context.GRAPH_MODE, device_target=args_opt.platform, device_id=0)
    net = ShuffleNetV2(n_class=cfg.num_classes)
    ckpt = load_checkpoint(args_opt.checkpoint)
    load_param_into_net(net, ckpt)
    net.set_train(False)
    dataset = create_dataset(args_opt.dataset_path, False, 0, 1)
    loss = CrossEntropySmooth(sparse=True, reduction='mean',
                              smooth_factor=0.1, num_classes=cfg.num_classes)
    eval_metrics = {'Loss': nn.Loss(),
                    'Top1-Acc': nn.Top1CategoricalAccuracy(),
                    'Top5-Acc': nn.Top5CategoricalAccuracy()}
    model = Model(net, loss, optimizer=None, metrics=eval_metrics)
    metrics = model.eval(dataset)
    print("metric: ", metrics)
