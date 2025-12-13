FROM espressif/idf:v5.5.1

WORKDIR /project
COPY . /project

ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8

RUN echo "source /opt/esp/idf/export.sh" >> /root/.bashrc

RUN apt-get update && \
  apt-get install -y wget unzip && \
  rm -rf /var/lib/apt/lists/* && \
  T_VERSION=1.14.1 && \
  wget https://releases.hashicorp.com/terraform/${T_VERSION}/terraform_${T_VERSION}_linux_amd64.zip && \
  unzip terraform_${T_VERSION}_linux_amd64.zip && \
  mv terraform /usr/local/bin/terraform && \
  rm terraform_${T_VERSION}_linux_amd64.zip