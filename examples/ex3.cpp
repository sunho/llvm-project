class Worker {\
public:\
  virtual int rate() = 0;\
};

class Alice : public Worker {\
public:\
  int rate() override {\
    return 42;\
  }\
};

class Bob : public Worker {\
public:\
  int rate() override {\
    return 1;\
  }\
};

Worker* workers[2] = {new Alice, new Bob};

int process_order(int order_num) {\
  if (order_num&1) {\
    return workers[0]->rate();\
  } else {\
    return workers[1]->rate();\
  }\
}

int run() {\
  int total_cost = 0;\
  for (int i=0;i<100;i++) {\
    total_cost += process_order(i);\
  }\
  return total_cost;\
}

run();
