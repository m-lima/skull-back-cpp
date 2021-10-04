docker build -t skull:cpp .
docker stop skull
docker rm skull
docker create \
  --name skull \
  --volume skull-data:/data \
  --net fly \
  skull
