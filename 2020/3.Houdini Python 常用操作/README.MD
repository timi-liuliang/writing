# Houdini Python 基本操作

## 1.获取节点
1. 通过 [hou.pwd()](https://www.sidefx.com/docs/houdini/hom/hou/pwd.html) 获取当前节点, 与 hou.node(".") 等价
```python
# get current node
node = hou.pwd()
```
2. 使用绝对路径获取节点
```python
# get node by absolute path, path start with '/'
node = hou.node("/obj/path1/path2/node_name")
```
3. 使用相对路径获取节点
```python
# get node by relative path (relative to this python node)
node = hou.node("../path1/path2/node_name")
```

## 2.获取结点参数值
1. 使用[hou.ch()](https://www.sidefx.com/docs/houdini/hom/hou/ch.html) 获取参数值
```python
# method 1
param_value = hou.ch("../param_name")
```
2. 使用 [hou.evalParm()](https://www.sidefx.com/docs/houdini/hom/hou/evalParm.html) 获取参数值   
```python
# method 2
param_value = hou.evalParm("../param_name")
```
3. 先获取节点，再通过节点函数 parm 获取参数值
```python
# method 3
node        = hou.node("node_name")
param_value = node.parm('param_name').eval()
```

## 3.如何查看参数名称
1. 移动鼠标到参数上，通过查看StatusTips获取参数名称   
![get param name](https://raw.githubusercontent.com/timi-liuliang/writing/master/2020/3.Houdini%20Python%20%E5%B8%B8%E7%94%A8%E6%93%8D%E4%BD%9C/get_param_name_1.png)   
2. 通过代码智能提示获取参数名称   
![get param name](https://raw.githubusercontent.com/timi-liuliang/writing/master/2020/3.Houdini%20Python%20%E5%B8%B8%E7%94%A8%E6%93%8D%E4%BD%9C/get_param_name_2.png)   