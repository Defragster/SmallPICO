void logMemory() {
#ifdef BOARD_HAS_PSRAM
  Serial.printf("Used PSRAM: %d\n", ESP.getPsramSize() - ESP.getFreePsram());
  uint32_t* psdRamBuffer;
  uint32_t allocSize = ESP.getFreePsram();
  do {
    allocSize -= 1024;
    psdRamBuffer = (uint32_t*)ps_malloc( allocSize );
  } while ( 0 == psdRamBuffer && allocSize > 20480 );
  Serial.printf("*psdRamBuffer: %p MAX size = %u\n", psdRamBuffer, allocSize );
  if ( 0 == psdRamBuffer ) {
    Serial.printf("FAIL NULL POINTER :: *psdRamBuffer: %p size = %u\n", psdRamBuffer, allocSize );
    return;
  }
  else {
    Serial.printf("*psdRamBuffer: %p size = %u\n", psdRamBuffer, allocSize );
  }
  Serial.printf("Used PSRAM: %d\n", ESP.getPsramSize() - ESP.getFreePsram());
  Serial.printf( "%d Bytes PSRAM to test ps_malloc()\n", allocSize );
  testMemory( psdRamBuffer, allocSize / sizeof(uint32_t ) );
  free(psdRamBuffer);
  Serial.printf("Used PSRAM: %d\n", ESP.getPsramSize() - ESP.getFreePsram());
#endif
}
void testMemory( uint32_t* psdRamBuffer, uint32_t testSize ) {
#ifdef BOARD_HAS_PSRAM
  for ( uint32_t ii = 0; ii < testSize; ii += 1 ) {
    psdRamBuffer[ii] = ii;
  }
  int jj = 0;
  for ( uint32_t ii = 0; ii < testSize; ii += 1 ) {
    if ( psdRamBuffer[ii] != ii ) jj++;
  }
  if ( 0 == jj ) {
    Serial.printf( "All Good! testSize DWORDS=%u\n", testSize );
  }
  else {
    Serial.printf( "%d Fails to compare! (test)\n", jj );
  }
#endif
}
