# 咱也不知道，咱也不敢问

### 看我表演我的传统艺能：解体 ——Amagi_Yukisaki(项目组RBQ)

### 所有成员请及时同步master分支

## Master Changelog From 2020.06.11(众所周知这是个栈)

### 2020.06.13

后端`Ctrl+C`不再导致数据损毁

修改了`.gitignore`以忽略编译生成文件

前后端结合代码Debug

### 2020.06.12

前后端结合代码

### 2020.06.11

修改了`.gitignore`

删除了测试数据(导致评测超时)和根目录中的`main.cpp`


##Todo

#### Backend

- [ ] B+树

- [ ] B+树缓存(基于`map`实现`LRU`算法，读写同池，懒惰写入(被换出或程序结束时进行写入)) -> 队长负责？

- [ ] 日志式操作？

#### Frontend

- [ ] `modify_profile`不可用

- [ ] 所有需要多行输出的操作`(query_train, query_order, query_transfer, query_ticket)`

- [ ] 输入合法性检查

- [ ] 美化(添加图片等)

- [ ] 细节调整：显示用户`name`而不是`username`
