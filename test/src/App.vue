<script setup lang="ts">
import axios from 'axios';
import { Label, LabelBox, LabelFont, LabelText } from './DSLabel';

var label = new Label("测试标签",70, 60, 2, 0, 1         );
var box = new LabelBox({
    x_start: 20,
    y_start: 20,
    x_end: 620,
    y_end: 460,
    lineWidth: 2,
});


var text = new LabelText({
    x_start: 20,
    y_start: 20,
    x_end: 620,
    y_end: 460,
    font: LabelFont.TSS24,
    texts: [
        "JKHJKJ和卡拉数据的卡拉就是打开链接看见了快速减肥的绿卡激发了开始减肥了解放昆仑山JFK垃圾饭卡减肥咖啡咖啡可乐就",
        "ahdoiajdiajdkjakdj哈伦裤是多久啊了空间打开链接啊看到了",
        "在时间和幅度卡死就回家贷记卡的绿卡",
        "asdjklasjdklasjd"
    ],
});
label.elements.push(box)
label.elements.push(text)
var labels=[label]
axios.post("http://127.0.0.1:16500/print", JSON.stringify(labels))

</script>

<template>

</template>

<style scoped></style>
