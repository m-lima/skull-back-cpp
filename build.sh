docker build -t skull .
docker stop skull
docker rm skull
docker create \
  --name skull \
  --volume "${PWD}/data":/data \
  --net fly \
  skull
