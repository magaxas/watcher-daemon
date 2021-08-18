# Watcher daemon

### Installation:
```
make
make install
```

### Debugging
Execute the command below and then start gdb:
```
make debug
```

### Config
Configuration file in this repository is given as an example:
- You can delete it and let the program load default configuration
- You could edit it according to your needs

### Use

```
sudo systemctl start watcher-daemon.service
ps aux | grep watcher-daemon
cat /var/log/watcher-daemon/logger.log
```