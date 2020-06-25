# 实现技术细节

## B+树

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

## B+树(含缓存)

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


## 后端（src文件夹下的内容）
#### 数据库

`database<type_userName, type_user> Users; // 用户的的相关信息`

`database<type_userName, int> Cur_users; // 仅判断是否已登录（value恒为0），查询详细信息还要在Users中查询 `

`database<type_trainID, type_train> Trains_base; // 列车的基本信息`

`database<type_runtimeID, type_train_release> Trains_released; // 已发布列车的车票信息等`

`database_cached<type_stationName_trainID, type_ticket_value> Database_query; // 查票数据库`

`database<type_userName_orderID, type_order> Database_orders; // 各用户的订单信息`

`database<type_queue_key, type_userName_orderID> Database_queue; // 候补队列`

#### 数据库具体解释

`type_trainID`和`type_userName`是封装好的定长字符串。

`type_user`类中存储一个用户T的全部信息。

`type_train`类中存储一辆列车的全部信息（除具体座位数）。

`type_runtimeID`类即包含一个trainID字符数组和日期。

`type_train_release`存储已发布车次的具体座位信息，它包含函数`queryseats(st, ed)`，返回区间`(st, ed)`最大可用票数。

`type_stationName_trainID`存储某列车经过的车站和该车次id号，用于查票和查询换乘。

`type_ticket_value`存放用于查票的值，具体包括列车的运营日期范围、该站的编号、到达与离开该站的时间（相对），该站的票价前缀和。

`Database_stations`的value为一个std::pair，分别是该车次的runtimeID和该站在这个车次中的编号（0-base）。

`type_userName_orderID`存储用户`id`和该用户的订单`id`（对每个用户从1开始）。

`type_order`存储一个订单的所有信息，包括订单状态等。

`Database_queue`：B+树模拟队列，避免循环不必要候补。

`type_queue_key`包含车次的runtimeID和全局订单编号。（全局订单编号另开了一个file_totalID文件显式存储，每次更新即时写入文件）

#### 杂项

`Ticket::datentime`是用于存储时间和日期的自定义类，后端中的时间、日期都是以这种形式存储，具体成员为`date`和`minu`（均为int型，`date`是从2020年1月0日算起的天数，minu将小时和分钟整合在一起）。

tools.hpp中实现了数个core中用到的小工具函数，不再一一列数。

**为保证运行速度，后端默认指令参数是合法的，不进行任何参数合法性检查。后端仅有非法指令、缺参数、日期越界等异常抛出，具体见src/exceptions.hpp文件。**

#### 成员函数的实现方法简介

同[TicketSystem-2020](https://github.com/oscardhc/TicketSystem-2020)要求函数。返回文档要求的string为答案。

具体实现：

`add_user`：添加用户。存入`Users`数据库。

`login`：该用户登录。存入`Cur_users`数据库。

`logout`：该用户登出。在`Cur_users`数据库中删除用户。

`query_profile`：在`Users`数据库中查询该用户。

`modify_profile`：在`Users`数据库中修改该用户的数据。

`add_train`：添加车次。信息全部存入`Trains_base`数据库。

`release_train`：发布车次。对运营日期范围内的每一天在`Trains_released`中存入初始化的座位数。

`query_train`：查询车次。在`Trains_base`中查询出车次的基本信息，若已发布则再在`Trains_released`中查询出具体座位情况。

`delete_train`：删除未发布的车次。在`Trains_base`中删除车次。

`query_ticket`：查询某天从某站到某站的车票。在`Database_query`中range2出所有经过这两站的列车id，由于range的有序性，可用双指针滑动来匹配id相符的车次，若日期条件符合则放入临时数组。待匹配结束，将临时数组按time或cost排序，输出结果。

`query_transfer`：查询一次换乘下的车票。在`Database_query`中range2出所有经过这两站的列车id；对每个经过起始站的车次，枚举它之后经过的车站作为可能的中间站，再range2出所有经过这个中间站的车次，运用同`query_ticket`的双指针滑动的方法找到同样经过终点站的车次作为第二趟车次。若日期、时间合法，则这个方案可行。最终输出time/cost最优的一个可行换乘方案。

（注：因为`type_ticket_value`的设计，查票和查换乘均不用在`Trains_base`中查询车次的全部信息。这是提速的一个关键）

`buy_ticket`：买票操作。在`Trains_base`中查询出该车次的全部信息，找到这两站的在其中的编号；若剩余票数足够则直接购买，更新总购买ID和用户的局部购买ID，更新该车次的座位信息；若不够但允许候补则添加进`Database_queue`中；不管怎么样，只要操作成功，都要在`Database_orders`中添加订单。

`query_order`：查询某用户的所有订单。直接在`Database_orders`中range出订单即可。

`refund_ticket`：退票。若本来就是候补状态，则直接删去候补；若购买成功，座位复原，range出该车次的候补队列，座位足够则候补购票成功，修改相关数据库。

`clean`：清空各数据库，总订单ID清零。

`exit`：清空`Cur_users`，返回"bye"，在主程序中退出程序。

## 前端

基于MDUI，所有静态文件存储于`https://cdn.yukisaki.io:2333/mdui`

通过管道与后端通讯，静态打表文件存储于`ticket.py`内的`Constant`类中。

奇怪的打表与模板化系统。（甚至只有5个`html`）

所有静态数据打表在`ticket.Constant`类中，所有`form`, `table`共用相同html模板，通过`Jinja2`进行展开，`html`中特判需要特殊显示的字段（比如非文本输入框的`input`段，需要额外`button`的`table`栏）。

对需要`post`方式提交的数据，通过隐藏`form`形式。`table`排版时仅放`button`，通过`form_id`联系相应`form`。

基于正则表达式检查指令合法性，指令要求同助教`readme`，所有正则表达式打表于`ticket.InputValidator类中`。

实现登录系统，通过下发`cookie`(一个`uuid4`)实现用户区分。

维护`uuid->(username, friendly_name, privilege)`, `username->uuid`映射，实现权限控制。

普通用户被管理员修改密码后，所在登录设备自动登出。

维护已登录用户的`username->password`映射，在提交后端前进行密码查验，实现二次登录时允许第二个设备登录并登出第一个设备。

基于暴力文本查询实现`List User`和`List Train`，个人认为不需要优化，反正也是`O(n)`(只是修改复杂度有点难看而已)。
