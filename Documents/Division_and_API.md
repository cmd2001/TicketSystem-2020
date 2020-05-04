# 火车票管理系统大作业 文档

## 人员分工

暂定将将整体项目分为三个子部分，分别为：(后面填对应GitHub id)

前端&文档(人员A)：

数据库(B+树)(人员B)：

后端程序(除B+树外全部算法)(人员C)：aik2mlj

## 程序架构

```mermaid
graph LR
D[database.hpp] -->A[core.hpp]
A --> C[main.cpp]
C --> J(评测机)
A --> B[backend.cpp]
B --> F(前端GUI)
```

其中`databse.hpp​`为人员B所完成数据库核心。

`core.hpp`为人员C所完成的算法程序（注意要封装为对象，在构造、析构函数打开、关闭相应文件）。

`main.cpp`为人员C所完成的后端主程序，**其中仅包含创建`core.hpp`中类的操作及必要IO，其余所有算法均在对应类中实现！**

`backend.cpp`为人员A所做的后端程序，负责与前端程序通讯。

## 约定接口

### `database.hpp`

实现一个B+树。

**只允许访问磁盘上的单个文件！**

支持以下操作：

`Database<type1, type2>(Filename)`：创建`Key`类型为`type1`，`Value`类型为`type2`的Database类并打开名为`Filename`的数据库文件。

`insert(Key, Value)`：向B+树内插入`(Key, Value)`键值。`Key`为定义了`<`运算的可排序类，`Value`为定义了`==`运算的可比较类，`Value`为定长类型（即：不同的`sizeof Value`为不变常数）。**注意：不同元素的`Key`可以重复！**

`erase(Key, Value)`：删除`(Key, Value)`键值，返回值为布尔型，表示是否删除成功。**（如果找不到相应元素则返回删除失败）**

`modify(Key, Old_value, New_value)`：将`(Key, Old_Value)`修改为`(Key, New_value)`，返回值为布尔型，表示是否修改成功。失败条件同上。

`query(Key)`：查询`Key`对应的`Value`。**保证此时的Key不出现重复！**返回`std::pair<bool, typeof Value>`，表示查询是否成功及查询到的`Value`值。失败条件同上。

`range(Key1, Key2)`：查询区间`[Key1, Key2]`对应的全部`Value`，返回`List<typeof Value>`，表示查询到的值。**其中`List`为自定义的列表类，支持迭代器遍历。**（为什么不使用`stl`？因为不允许。为什么能用`std::pair`？因为`std::pair`在`iostream`中有包含）**为什么不返回对应的`Key`？因为可以保证Key在Value中已被包含。**（这一点在core.hpp的实现中将会提及）

### `core.hpp`

实现`Ticket`类。

其中包含以下数据成员：

`Database<type_username, type_user> users(file_user);`

`Database<type_trainID, type_train> trains_unrelease(file_unreleased), trains(file_trains);`

`Database<type_statinName_startTime, type_trainID> database_station(file_statinName);`

`Database<type_userID_orderID, type_order> database_order(Order)`

`type_user`类中存储一个用户的全部信息，`type_train`类中存储一次列车的全部信息。

`type_train`需包含函数`query(st, ed)`，返回区间`(st, ed)`最大可用票数。

`type_statinName_startTime`存储某列车经过的车站和它经过这个车站的时间，**仅存储仍有剩余座位的列车**。

`type_userID_orderID`存储用户`id`和该用户的订单`id`（对每个用户从1开始）。

包含以下函数：

[TicketSystem-2020](https://github.com/oscardhc/TicketSystem-2020)所要求的全部函数。

实现思路为：

对于查询列车的操作，先查询出`id`，再去另一个数据库中用`id`查询该次列车全部信息。

修改操作所有数据库同步！

`queryTrain(id)`：返回值为`type_train`返回`train_id`为`id`的车次的全部信息。**保证查询车次一定存在**。（评测中不会涉及，前端专用）
