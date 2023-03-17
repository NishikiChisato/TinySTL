# TinySTL

## 简介

基于 `C++11` 的 `TinySTL`。这是我的第一个上手的项目，文档与注释均采用中文。因为是新手，所有代码中难免会有不规范和错误的地方，还希望得到指正。目前正在编写中，期望在今年五月中旬完结。

## 目前进度

* `alloc.h`，目前已结束

* `iterator.h`，还差反向迭代器`reverse_iterator`
  * 完成度 `80%`

* `type_trait.h`，目前已结束

* `construct.h`，目前已结束

* `uninitialized.h`，还差`uninitialized_copy`

* `algobase.h`，只有基础函数部分
  * 完成度 `5%`

## 项目难点记录

*  `iterator_traits` 需要针对不同类型使用 `Partial Specialization` 

*  `copy` 定义中针对不同场景的 `generalization`, `specialization`, `refinement`

## 疑问

* `iterator.h` 中的 `distance_type` 与 `value_type` 的返回值为什么是指针

回答：迭代器所指对象型别为 `T` 的话，此处应当返回 `T*` 用以表示指向型别 `T` 的迭代器；除此之外，此函数的接口均接受 `T*` 型别的对象