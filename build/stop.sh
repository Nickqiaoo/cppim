ps aux | grep logicserver | awk '{print $2}' | xargs kill
ps aux | grep gateserver | awk '{print $2}' | xargs kill
ps aux | grep jobserver | awk '{print $2}' | xargs kill
