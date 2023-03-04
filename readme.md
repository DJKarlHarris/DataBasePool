# This is a dbpool project
## 连接池
### 1. 模型：生产者消费者模型
#### 生产者：生产连接 
#### 消费者: 使用数据库的客户端
### 2. 控制连接数量的逻辑
#### 1. 连接池内连接过多
#### 时间戳记录连接的空闲时间，若空闲时间大于指定时长，就销毁掉该连接
#### 2. 连接池内连接过少
#### 直接新增连接
## 涉及技术点
1. 互斥锁、条件变量（由于模型逻辑简单，所以生产者和消费者共用**一把互斥锁**和**一个条件变量**，不影响线程池逻辑)
2. 队列:存储数据库连接
3. 单例模式（**懒汉模式**中注意线程安全问题，要加锁,利用C++11中的static来实现)
    1. 静态局部变量:生命周期为整个程序周期,在线程池对外的接口getConnectionPool函数中声明,并且由于C++11特性，该声明只会有一次
    2. 私有成员 
4. 通过jason来实现数据库连接信息的传递(通过**vcpkg**包管理jasoncpp库)
5. 回收线程有两种方式
    1. 函数控制回收: 当使用连接的线程完成时，调用该函数将连接放回队列中 
    2. shared_ptr<T>控制回收: 将回收连接的逻辑写在共享指针的**删除器**中 

