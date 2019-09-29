docker build -t skull .
docker stop skull
docker rm skull
. ./cfg/data_location.env
docker create \
  --name skull \
  --volume "${DATA_LOCATION}":/data \
  --net fly \
  skull
