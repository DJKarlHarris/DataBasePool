this is a dbpool project
## 连接池
### 模型：生产者消费者模型
### 生产者：生产连接
### 消费者: 使用数据库的客户端
### 涉及技术点
1. 互斥锁 
2. 条件变量
3. 队列
4. 单例模式（懒汉模式中注意线程安全问题，要加锁,利用C++11中的static来实现)
    1. 静态局部变量
    2. 私有成员 
5. 通过jason来实现数据库连接信息的传递(通过vcpkg包管理jasoncpp库)
## 控制连接数量的逻辑
### 连接池内连接过多
#### 时间戳记录连接的空闲时间，若空闲时间大于指定时长，就销毁掉该连接
### 连接池内连接过少
#### 新增连接
