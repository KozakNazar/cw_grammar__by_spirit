NAME _PROGRAM ; 
BODY DATA INTEGER16 _VALUEIN , _DEVIDER , _REMAIND , _CYCLEAC , _CYCLEBC , _CYCLECC ;
  GET ( _VALUEIN )
  _VALUEIN << _VALUEIN + 1
  FOR  _CYCLEAC << 0 TO 32767 DO
    _DEVIDER << _VALUEIN - 1
    FOR  _CYCLEBC << 0 TO 32767 DO 
      IF _DEVIDER >= 2 THEN ; ELSE GOTO endcycleb ;
      _REMAIND << _VALUEIN
      FOR  _CYCLECC << 0 TO 32767 DO
        IF _REMAIND >= _DEVIDER THEN ; ELSE GOTO endcyclec ;
        _REMAIND << _REMAIND - _DEVIDER
      ;
      endcyclec :
      IF _REMAIND == 0 THEN GOTO endcycleb ;
      _DEVIDER << _DEVIDER - 1
    ;
    endcycleb :
    IF _DEVIDER == 1 THEN GOTO endcyclea ;
    _VALUEIN << _VALUEIN + 1
  ;
  endcyclea :
  PUT ( _VALUEIN )
END
