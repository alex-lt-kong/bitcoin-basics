#ifndef TX_H
#define TX_H

#include <stdint.h>

class Tx {
private:
  int version = -1;
  uint8_t** tx_ins = nullptr;
  uint8_t** tx_outs = nullptr;
  unsigned int locktime = -1;
  bool is_testnet = false;
protected:
public:
  /**
   * @brief Constructor
   * @param version
   * @param tx_ins the memory will be managed by this Tx instance after being passed to it
   * @param tx_outs the memory will be managed by this Tx instance after being passed to it
   * @param locktime
   * @param is_testnet
   */
  Tx(int version, uint8_t** tx_ins, uint8_t** tx_outs, unsigned int locktime, bool is_testnet);
  void to_string();
  /**
   * @brief 
   * @param serialization the memory will NOT be managed by the method.
  */
  static uint32_t parse(uint8_t* serialization);
  ~Tx();
};

#endif