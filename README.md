# TinySTL

## 简介

基于 `C++11` 的 `TinySTL`。这是我的第一个上手的项目，文档与注释均采用中文。因为是新手，所有代码中难免会有不规范和错误的地方，还希望各位指正。目前正在编写中，期望在今年五月中旬完结。

## 目前进度

* `alloc.h`，已完成
* `iterator.h`与`type_traits.h`，还差反向迭代器`reverse_iterator`
  * 完成度 `80%`
* `construct.h`，目前正在编写
  * 完成度 `30%`
* `uninitialized.h`，目前正在编写
  * 完成度 `30%`

## 项目难点记录

*  为什么 `iterator_traits` 需要使用 `Partial Specialization`

## 疑问

* `iterator.h` 中的 `distance_type` 与 `value_type` 的返回值为什么是指针

回答：迭代器所指对象型别为 `T` 的话，此处应当返回 `T*` 用以表示指向型别 `T` 的迭代器；除此之外，此函数的接口均接受 `T*` 型别的对象