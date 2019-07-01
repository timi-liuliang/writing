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

## 4.模拟按钮点击
为了保证可编译工具管线全自动批处理，有时需要通过代码模拟按钮的点击。   
![save to disk](https://raw.githubusercontent.com/timi-liuliang/writing/master/2020/3.Houdini%20Python%20%E5%B8%B8%E7%94%A8%E6%93%8D%E4%BD%9C/save_to_disk_button.png)  
图：通过python 模拟点击 Save To Disk 按钮   

1. 先获取对应节点，再通过节点的 parm 函数获取按钮, 然后通过pressButton模拟按钮点击
```python
hf_output = hou.node("../heightfield_output1/")
hf_output.parm("execute").pressButton()
```
2. 通过 hou.parm 直接获取按钮, 再调用 pressButton 模拟点击
```python
hou.parm("../heightfield_output1/execute").pressButton()
```

## 5.如何处理 channel 脚本超过一行的情况

1. 鼠标点击参数的编辑区域(下图绿色区域)，进入编辑状态; 再使用快捷键 CTRL+E 打开脚本编辑对话框。
2. 最后使用 return 返回最终结果
![channel](https://raw.githubusercontent.com/timi-liuliang/writing/master/2020/3.Houdini%20Python%20%E5%B8%B8%E7%94%A8%E6%93%8D%E4%BD%9C/multiline_channel.png)  

## 6.添加属性
我们一般通过 hou.geometry 的 addAttrib(type, name, default_value, transform_as_normal=False, create_local_variable=True) 方法添加属性。addAttrib方法的变量虽然很多，不过大部分都有默认值，一般我们只需要关心属性的 类型、名称、值.
对于数组类型，则可以通过 addArrayAttrib 方法来实现
```python
node = hou.pwd()
geo  = node.geometry()
 
# add global or detail attribute
geo.addAttrib(hou.attribType.Global, "x_coord", 3)
geo.addAttrib(hou.attribType.Global, "y_coord", 3)

# add array attribute
geo.addArrayAttrib(hou.attribType.Global, "culture", hou.attribData.String, 1)
geo.setGlobalAttribValue("culture", tuple(["Integrity", "Propactive", "Collaboration", "Innovation"]))
```
执行代码后，可以在 Geometry Spreadsheet 中查看结果。

![Geometry Spreadsheet](https://raw.githubusercontent.com/timi-liuliang/writing/master/2020/3.Houdini%20Python%20%E5%B8%B8%E7%94%A8%E6%93%8D%E4%BD%9C/addattrib.png)

## 参考
[1]. [HoudiniTricks](https://houdinitricks.com/category/quicktips/). [CREATING ATTRIBUTES WITH PYTHON IN HOUDINI](https://houdinitricks.com/creating-attributes-with-python-in-houdini/) (2018)