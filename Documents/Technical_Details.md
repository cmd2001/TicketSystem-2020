# 实现技术细节

### B+树

两种节点，左自右开，`0-base`，不同使用序列。

**保证不同元素的Key不会重复**

#### 成员函数：

`Database<type1, type2>(Filename)`：创建`Key`类型为`type1`，`Value`类型为`type2`的Database类并打开名为`Filename`的数据库文件。

`insert(Key, Value)`：向B+树内插入`(Key, Value)`键值。`Key`为定义了`<`运算的可排序类，`Key, Value`为定长类型（即：不同的`sizeof Key, sizeof Value`为不变常数）。**注意：不同元素的`Key`不会重复！**

`erase(Key)`：删除`(Key, Value)`键值，返回值为布尔型，表示是否删除成功。**（如果找不到相应元素则返回删除失败）**

`modify(Key, New_value)`：将`(Key, ?)`修改为`(Key, New_value)`，返回值为布尔型，表示是否修改成功。失败条件同上。

`query(Key)`：查询`Key`对应的`Value`。返回`std::pair<bool, typeof Value>`，表示查询是否成功及查询到的`Value`值。失败条件同上。

`range(Key1, Key2)`：查询区间`[Key1, Key2]`对应的全部`Value`，返回`List<typeof Value>`，表示查询到的值。**其中`List`为自定义的列表类，支持迭代器遍历。**（为什么不使用`stl`？因为不允许。为什么能用`std::pair`？因为`std::pair`在`iostream`中有包含）（这一点在core.hpp的实现中将会提及）

`range2(Key1, Key2)`: 意义与`range`相似，与之不同的是返回`List<pair<typeof Key, typeof Value>>`。

`clear()`：清空名为`Filename`的数据库文件。

`empty()`：返回bool类型，表示该数据库是否为空。

`size()`：返回size_t类型，表示当前B+树的大小。

`save()`: 提供一个flush的显式接口。

### B+树(含缓存)

双`map`实现`LRU`算法，读写缓存共用一个`map`，注意标记合并。

`range`函数：两次查询，归并排序。

#### 成员函数：

`Database_cached<type1, type2>(Filename(, _cache_Size))`：创建`Key`类型为`type1`，`Value`类型为`type2`的Database类并打开名为`Filename`的数据库文件，缓存大小为`_cache_Size`。当`_cache_Size=0`时不进行缓存（直接调用B+树同名函数）。

`insert(Key, Value)`：同B+树`insert`。**保证操作不会失败**

`erase(Key)`：同B+树`erase`。**保证删除成功(指定元素存在)**

`modify(Key, New_value)`：同上`modify`。**保证操作成功**

`query(Key)`：同B+树`query`。**不保证查询成功**

`range(Key1, Key2)`：同B+树`range`。

`range2(Key1, Key2)`: 同B+树`range2`

`clear()`：清空名为`Filename`的数据库文件。

`empty()`：返回bool类型，表示该数据库是否为空。

`size()`：返回size_t类型，表示当前B+树的大小。


### 后端

**为保证运行速度，后端默认指令合法，不进行任何合法性检查。**

#### 成员函数

同[TicketSystem-2020](https://github.com/oscardhc/TicketSystem-2020)要求函数。返回string为答案。

#### 特别注意：

`query_ticket`：查询过两站的列车，双指针滑动找对应。注意列车不能倒开。

`query_transfer`：查询过两站的列车，枚举起始站列车，查询信息，找中间站，查询过中间站列车，与终点站双指针对比。

`queue`：~~B+树模拟队列~~，列车的`runtimeID(TrainID+Date)`和用户购票`ID`作为`key`，避免循环不必要候补。

### 前端

基于MDUI，所有静态文件存储于`https://cdn.yukisaki.io:2333/mdui`

奇怪的打表与模板化系统。（甚至只有4个`html`）

通过管道与后端通讯，静态打表文件存储于`ticket.py`内的`Constant`类中。

**前端检查指令合法性。**