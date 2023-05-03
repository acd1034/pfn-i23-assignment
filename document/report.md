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
  from_nodeのoutput_valuesのi番目の要素を削除
  to_nodeのinput_valuesのj番目の要素を削除
  return

/// valueのsource_nodeをto_nodeに変更する
function relink_source_node(
  std::shared_ptr<Value> value,
  std::shared_ptr<Node> to_node):
  std::shared_ptr<Node> from_node ← valueのsource_nodeのstd::shared_ptr
  usize i ← from_nodeのoutput_values内でのvalueの位置
  from_nodeのoutput_valuesのi番目の要素を削除
  to_nodeのoutput_valuesの末尾にvalueを追加
  valueのsource_nodeにto_nodeを代入
  return
```

```cpp
Graph eliminate_nop(Graph graph):
  usize i ← 0
  while i != graph.nodes.length:
    std::shared_ptr<Node> node = graph.nodes[i]
    bool is_nop ← node->name == "NOP" and node->input_values.length == 1 and node->output_values.length == 1
    if (not is_nop)
      i ← i + 1
      continue
    std::shared_ptr<Value> value ← node->input_values[0]
    std::shared_ptr<Node> prev_node ← value->source_node
    // ... -> prev_node -(value)-> node -> next_node -> ...
    disconnect_nodes(value)
    // ... -> prev_node
    //             node -(value2)-> next_node -> ...
    std::shared_ptr<Value> value2 ← node->output_values[0]
    relink_source_node(value2, prev_node)
    // ... -> prev_node -> next_node -> ...
    //             node
    graph.nodesのi番目の要素を削除
    iに削除した要素の次の要素の位置を代入
    // ... -> prev_node -> next_node -> ...
```
