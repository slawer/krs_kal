#include "DB.h"
#include "..\DBView.h"

void DBGET_Get(DB_RequestParamValuesPacket* packet);
void DBGET_Start();
void DBGET_HandlePacket(DB_TransferParamValuesPacket* transfer_packet, CMainView* view);
bool DBGET_RequestFinished();

////////////////////////////////////////////////////////////////////////////////
// end