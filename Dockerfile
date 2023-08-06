FROM alpine:latest

RUN apk add build-base make cmake

COPY . .

RUN rm -rf build/*
RUN cmake -B build
RUN cd build
RUN make
RUN chmod +x dser

WORKDIR /usr/src/app

EXPOSE 3000

CMD ["./build/dser"]

