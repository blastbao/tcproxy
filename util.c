#include "util.h"

void tp_log(const char *fmt, ...) { 
  va_list  args;
  
  va_start(args, fmt);
  vfprintf(stderr, fmt, args);
  fprintf(stderr, "\n");
  va_end(args);
}

int setnonblock(int fd) {
  int opts;
  if ((opts = fcntl(fd, F_GETFL)) < 0) {
    FATAL("GET FLAG");
  }
  opts = opts | O_NONBLOCK;
  if(fcntl(fd, F_SETFL, opts) < 0) {
    FATAL("SET NOBLOCK");
  }
  return 0;
}

int bind_addr(const char *host, short port) {
  int fd, on = 1;
  struct sockaddr_in addr;

  if ((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
    FATAL("socket");
  }

  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(int)) ==-1) {
    FATAL("set sock opt");
  }

  memset(&addr, 0, sizeof(struct sockaddr_in));
  addr.sin_family = PF_INET;
  addr.sin_port = htons(port);
  if (strcmp(host, "any") == 0) {
    addr.sin_addr.s_addr = INADDR_ANY;
  } else {
    inet_aton(host, &addr.sin_addr);
  }

  if (bind(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1) {
    FATAL("bind");
  }

  if (listen(fd, 10) == -1) {
    FATAL("listen");
  }

  setnonblock(fd);

  return fd;
}

int connect_addr(const char *host, short port) {
  int fd;
  struct sockaddr_in addr;

  fd = socket(PF_INET, SOCK_STREAM, 0);
  addr.sin_family = PF_INET;
  addr.sin_port = htons(port);
  inet_aton(host, &addr.sin_addr);

  if (connect(fd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in)) == -1) {
    if (errno != EINPROGRESS) {
      FATAL("connect");
    }
  }

  return fd;
}

struct rwbuffer *rwb_new(size) {
  struct rwbuffer *buf = malloc(sizeof(struct rwbuffer));
  buf->data = malloc(size * sizeof(char));
  buf->size = size;
  buf->r = buf->w = 0;
  return buf;
}

int rwb_size_to_read(struct rwbuffer *buf) {
  if (buf->r <= buf->w) return buf->w - buf->r;
  else return buf->size - buf->r;
}

int rwb_size_to_write(struct rwbuffer *buf) {
  if (buf->r <= buf->w) return buf->size - buf->w;
  else return buf->r - buf->w;
}

char *rwb_read_buf(struct rwbuffer *buf) {
  return &buf->data[buf->r];
}

char *rwb_write_buf(struct rwbuffer *buf) {
  return &buf->data[buf->w];
}

void rwb_read_size(struct rwbuffer *buf, int size) {
  buf->r += size;
  if (buf->r == buf->size) buf->r = 0;
}

void rwb_write_size(struct rwbuffer *buf, int size) {
  buf->w += size;
  if (buf->w == buf->size) buf->w = 0;
}
