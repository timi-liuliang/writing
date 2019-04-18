# Box2D Joints 介绍

## 介绍
Box2D 共有11种Joints,分别是   
### 1.Distance Joint   
```cpp
b2DistanceJointDef jointDef;
jointDef.Initialize(myBodyA, myBodyB, worldAnchorOnBodyA, worldAnchorOnBodyB);
jointDef.collideConnected = true;
```

### 2.Friction Joint     
### 3.Gear Joint   
### 4.Motor Joint   
### 5.Mouse Joint   
### 6.Prismatic Joint   
### 7.Pulley Joint   
### 8.Revolute Joint  
```cpp
b2RevoluteJointDef jointDef;
jointDef.Initialize(myBodyA, myBodyB, myBodyA->GetWorldCenter());
```
### 9.Rope Joint   
### 10.Weld Joint   
### 11.Wheel Joint   

## 参考
[1]. Erin Catto(2007-2013).[Box2D v2.3.0 User Manual](https://box2d.org/manual.pdf)
[2]. Haziq Manap(2014).[Types of Joint in Industrial Robot](https://www.youtube.com/watch?v=SMcqUjQ2Swo)
[3]. Google(2018).[LiquidFun](http://google.github.io/liquidfun/)
