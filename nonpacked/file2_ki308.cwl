NAME _PROGRAM ; 
BODY DATA INTEGER16 _VALUEIN , _DEVIDER , _REMAIND , _CYCLEAC , _CYCLEBC , _CYCLECC ;
  GET ( _VALUEIN )
  1 + _VALUEIN >> _VALUEIN
  FOR  0 >> _CYCLEAC TO 32767 DO
    _VALUEIN - 1 >> _DEVIDER
    FOR  0 >> _CYCLEBC TO 32767 DO 
      IF _DEVIDER >= 2 THEN ; ELSE GOTO endcycleb ;
      _VALUEIN >> _REMAIND
      FOR  0 >> _CYCLECC TO 32767 DO
        IF _REMAIND >= _DEVIDER THEN ; ELSE GOTO endcyclec ;
        _REMAIND - _DEVIDER >> _REMAIND
      ;
      endcyclec :
      IF _REMAIND == 0 THEN GOTO endcycleb ;
      _DEVIDER - 1 >> _DEVIDER
    ;
    endcycleb :
    IF _DEVIDER == 1 THEN GOTO endcyclea ;
    1 + _VALUEIN >> _VALUEIN
  ;
  endcyclea :
  PUT ( _VALUEIN )
END
