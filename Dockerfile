FROM ubuntu:18.04

WORKDIR /app
RUN apt-get update
RUN apt-get install cmake -y

RUN apt-get install wget bzip2 unzip g++ m4 libboost-all-dev nasm -y

COPY ./libOTe ./libOTe

RUN cd libOTe/cryptoTools/thirdparty/linux && bash boost.get 
RUN cd libOTe/cryptoTools/thirdparty/linux && bash miracl.get 
RUN cd libOTe && cmake . 
RUN cd libOTe && make -j

COPY ./thirdparty ./thirdparty

RUN cd thirdparty/linux && bash gf2x.get 
RUN cd thirdparty/linux && bash gmp.get 
RUN cd thirdparty/linux && bash ntl.get

RUN apt-get install libxxhash0 libgivaro-dev liblinbox-dev libiml-dev -y

COPY . .
# RUN sleep infinity
RUN cmake .
RUN make -j
RUN sleep infinity
RUN ./bin/frontend.exe -u 