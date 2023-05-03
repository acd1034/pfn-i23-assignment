## 問 1

```cpp
class Node:
  String name
  List<std::shared_ptr<Value>> input_values
  List<std::shared_ptr<Value>> output_values

class Value:
  std::weak_ptr<Node> source_node
  std::weak_ptr<Node> target_node

class Graph:
  List<std::shared_ptr<Node>> nodes
```

## 問 2

```cpp
function connect_nodes(
  std::shared_ptr<Node> from_node,
  std::shared_ptr<Node> to_node):
  std::shared_ptr<Value> value ← from_nodeをsource_node,to_nodeをtarget_nodeとするValueのstd::shared_ptr
  from_nodeのoutput_valuesの末尾にvalueを追加
  to_nodeのinput_valuesの末尾にvalueを追加
  return

function disconnect_nodes(std::shared_ptr<Value> value):
  std::shared_ptr<Node> from_node ← valueのsource_nodeのstd::shared_ptr
  std::shared_ptr<Node> to_node ← valueのtarget_nodeのstd::shared_ptr
  usize i ← from_nodeのoutput_values内でのvalueの位置
  usize j ← to_nodeのinput_values内でのvalueの位置
  from_nodeのi番目の要素を削除
  to_nodeのj番目の要素を削除
  return

/// valueのsource_nodeをto_nodeに変更する
function relink_source_node(
  std::shared_ptr<Value> value,
  std::shared_ptr<Node> to_node):
  std::shared_ptr<Node> from_node ← valueのsource_nodeのstd::shared_ptr
  usize i ← from_nodeのoutput_values内でのvalueの位置
  from_nodeのi番目の要素を削除
  to_nodeのoutput_valuesの末尾にvalueを追加
  valueのsource_nodeにto_nodeを代入
  return
```

```cpp

```
