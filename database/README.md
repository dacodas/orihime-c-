# Overview 

Running the following almost works, but not quite

```
podman run \
       -p 30060:3006/tcp \
       -e MYSQL_ROOT_PASSWORD=temporary-test-password \
       orihime/mysql
```

## Networking issues 

Take a look at the `slirp4netns` process running through `htop`. In order to properly communicate with the container, I have to `nsenter -t $PID -n /bin/bash` and then start the `mysql` client process. Here, `PID` is the `pid` of the actual container process under `conmon`. 
