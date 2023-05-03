## 問 1

```cpp
class Node:
  name: String
  input_values: List of SharedObject of Value
  output_values: List of SharedObject of Value

class Value:
  source_node: WeakReference of Node
  target_node: WeakReference of Node

class Graph:
  nodes: List of SharedObject of Node
```

## 問 2

```cpp
function disconnect_nodes(value: SharedObject of Value):
  var from_node: SharedObject of Node ← value->source_node
  var to_node: SharedObject of Node ← value->target_node
  var i: usize ← from_node->output_values内でのvalueの位置
  var j: usize ← to_node->input_values内でのvalueの位置
  from_node->output_valuesのi番目の要素を削除
  to_node->input_valuesのj番目の要素を削除
  return

/// value->source_nodeをto_nodeに変更する
function relink_source_node(
  value: SharedObject of Value, to_node: SharedObject of Node):
  var from_node: SharedObject of Node ← value->source_node
  var i: usize ← from_nodeのoutput_values内でのvalueの位置
  from_node->output_valuesのi番目の要素を削除
  to_node->output_valuesの末尾にvalueを追加
  value->source_node ← to_node
  return
```

```cpp
function eliminate_nop(graph: Graph) -> Graph:
  var i: usize ← 0
  while i != graph.nodes.length do
    var node: SharedObject of Node ← graph.nodes[i]
    var is_nop: bool ← node->name == "NOP" and node->input_values.length == 1 and node->output_values.length == 1
    if not is_nop then
      i ← i + 1
      continue
    //        (i-1)                (i)     (i+1)
    // ... -> prev_node -(value)-> node -> next_node -> ...
    var value: SharedObject of Value ← node->input_values[0]
    var prev_node: SharedObject of Node ← value->source_node
    disconnect_nodes(value)
    // ... -> prev_node
    //             node -(value2)-> next_node -> ...
    var value2: SharedObject of Value ← node->output_values[0]
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
  from_node: SharedObject of Node, to_node: SharedObject of Node):
  var value: SharedObject of Value ← from_nodeをsource_node,to_nodeをtarget_nodeとするValue
  from_node->output_valuesの末尾にvalueを追加
  to_node->input_valuesの末尾にvalueを追加
  return
```

```cpp
function insert_nop_after_opa(graph: Graph) -> Graph:
  var i: usize ← 0
  while i != graph.nodes.length do
    var node: SharedObject of Node ← graph.nodes[i]
    var is_opa: bool ← node->name == "opA" and node->output_values.length == 1
    if not is_opa then
      i ← i + 1
      continue
    //        (i)     (i+1)
    // ... -> node -> next_node -> ...
    var nop: SharedObject of Node ← "NOP"をnameとするNode
    graph.nodesのi+1番目の位置にnopを挿入
    // ... -> node -(value)-> next_node -> ...
    //         nop
    var value: SharedObject of Value ← node->output_values[0]
    relink_source_node(value, nop)
    // ... -> node
    //         nop -> next_node -> ...
    connect_nodes(node, nop)
    //        (i)     (i+1)   (i+2)
    // ... -> node -> nop  -> next_node -> ...
    i ← i + 2
  return graph
```
