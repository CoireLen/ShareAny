# 编译安装说明

## Linux

从源码编译

```
cd ShareAny
mkdir build
cd build
cmake ../
make
cp ShareAny ../
cd ..
./ShareAny
```

### opesuse




## Q&A

##### SSL Private key file (server.key) must be unreadable for group and others

SSL 私钥文件 必须 其他组不可读

```
sudo chmod 700 ./server.key
```

##### Error: Could not bind to address 0.0.0.0 port 80: Could not listen on address.

在 linux 系统上 0~1024 端口 需要管理员权限

```
修改setting.json
"endpoint": "8080"
```
