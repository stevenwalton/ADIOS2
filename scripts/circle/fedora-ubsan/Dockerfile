FROM fedora:latest

# Install core dev packages
RUN dnf -y install gcc gcc-c++ gcc-gfortran cmake git make curl file \
        zlib zlib-devel bzip2-libs bzip2-devel hdf5 hdf5-devel \
        python3-devel python3-numpy zeromq zeromq-devel libubsan

# Cleanup headers and packages
RUN dnf clean all

# Create a non-root user to run the builds/tests
RUN export uid=1001 gid=1001 && \
    mkdir -p /home/adios2 && \
    echo "adios2:x:${uid}:${gid}:adios2,,,:/home/adios2:/bin/bash" >> /etc/passwd && \
    echo "adios2:x:${uid}:" >> /etc/group && \
    chown ${uid}:${gid} -R /home/adios2

# Misc cleanup of unneeded files
RUN rm -rfv /tmp/* \
  && rm -rf /var/cache/dnf

USER adios2
ENV HOME /home/adios2
WORKDIR /home/adios2
CMD /bin/bash
