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
function disconnect_nodes(std::shared_ptr<Value> value):
  std::shared_ptr<Node> from_node ← value->source_node
  std::shared_ptr<Node> to_node ← value->target_node
  usize i ← from_node->output_values内でのvalueの位置
  usize j ← to_node->input_values内でのvalueの位置
  from_node->output_valuesのi番目の要素を削除
  to_node->input_valuesのj番目の要素を削除
  return

/// value->source_nodeをto_nodeに変更する
function relink_source_node(
  std::shared_ptr<Value> value,
  std::shared_ptr<Node> to_node):
  std::shared_ptr<Node> from_node ← value->source_node
  usize i ← from_nodeのoutput_values内でのvalueの位置
  from_node->output_valuesのi番目の要素を削除
  to_node->output_valuesの末尾にvalueを追加
  value->source_node ← to_node
  return
```

```cpp
function eliminate_nop(Graph graph) -> Graph:
  usize i ← 0
  while i != graph.nodes.length do
    std::shared_ptr<Node> node ← graph.nodes[i]
    bool is_nop ← node->name == "NOP" and node->input_values.length == 1 and node->output_values.length == 1
    if not is_nop then
      i ← i + 1
      continue
    //        (i-1)                (i)     (i+1)
    // ... -> prev_node -(value)-> node -> next_node -> ...
    std::shared_ptr<Value> value ← node->input_values[0]
    std::shared_ptr<Node> prev_node ← value->source_node
    disconnect_nodes(value)
    // ... -> prev_node
    //             node -(value2)-> next_node -> ...
    std::shared_ptr<Value> value2 ← node->output_values[0]
    relink_source_node(value2, prev_node)
    // ... -> prev_node -> next_node -> ...
    //             node
    graph.nodesのi番目の要素を削除
    //        (i-1)        (i)
    // ... -> prev_node -> next_node -> ...
  return graph
```

## 問 3

```cpp
function connect_nodes(
  std::shared_ptr<Node> from_node,
  std::shared_ptr<Node> to_node):
  std::shared_ptr<Value> value ← from_nodeをsource_node,to_nodeをtarget_nodeとするValue
  from_node->output_valuesの末尾にvalueを追加
  to_node->input_valuesの末尾にvalueを追加
  return
```

```cpp
function insert_nop_after_opa(Graph graph) -> Graph:
  usize i ← 0
  while i != graph.nodes.length do
    std::shared_ptr<Node> node ← graph.nodes[i]
    bool is_opa ← node->name == "opA" and node->output_values.length == 1
    if not is_opa then
      i ← i + 1
      continue
    //        (i)     (i+1)
    // ... -> node -> next_node -> ...
    std::shared_ptr<Node> nop ← "NOP"をnameとするNode
    graph.nodesのi+1番目の位置にnopを挿入
    // ... -> node -(value)-> next_node -> ...
    //         nop
    std::shared_ptr<Value> value ← node->output_values[0]
    relink_source_node(value, nop)
    // ... -> node
    //         nop -> next_node -> ...
    connect_nodes(node, nop)
    //        (i)     (i+1)   (i+2)
    // ... -> node -> nop  -> next_node -> ...
    i ← i + 2
  return graph
```
