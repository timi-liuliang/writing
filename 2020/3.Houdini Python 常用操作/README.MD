# Houdini Python 基本操作

## 1.获取节点
```python
# get current node
node = hou.pwd()

# get node by absolute path, path start with '/'
node = hou.node("/obj/path1/path2/node_name")

# get node by relative path (relative to this python node)
node = hou.node("../path1/path2/node_name")

```

## 2.获取结点参数值
[hou.ch](https://www.sidefx.com/docs/houdini/hom/hou/ch.html)   
[hou.evalParm](https://www.sidefx.com/docs/houdini/hom/hou/evalParm.html)   

```python
# method 1
param_value = hou.ch("../param_name")

# method 2
param_value = hou.evalParm("../param_name")

# method 3
node        = hou.node("node_name")
param_value = node.parm('param_name').eval()
```

## 3.如何查看参数名称
1. 移动鼠标到参数上，通过查看StatusTips获取参数名称
2. 通过代码智能提示获取参数名称