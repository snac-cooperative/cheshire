
class Zoom {
    private:
    static variable options {}
    array xoptions {}
    variable errorCode {-1}
    variable errorMessage {-1}
    variable addInfo {-1}
    
    public:
    proc setOption {args} {
	source "setter.tcl"
    }
    proc getErrorCode {} {
	return $errorCode
    }
    proc getErrorMessage {} {
	return $errorMessage
    }
    proc getAddInfo {} {
	return $addInfo
    }

}
