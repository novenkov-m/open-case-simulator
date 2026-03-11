FROM alpine:latest

RUN apk add g++

WORKDIR /a

COPY /src /a

RUN g++ -std=c++20 -o game main.cc GameSimulator.cc Item.cc Container.cc WeaponCase.cc Shop.cc Player.cc

CMD ["./game"]
