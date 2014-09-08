package sdlip;

/**
 * This is the exception throws by SDLIP methods.
 */

public final class SDLIPException extends Exception {

  public static final short INVALID_REQUEST_EXC = 400;
  public static final short UNAUTHORIZED_EXC = 401;
  public static final short PAYMENT_REQUIRED_EXC = 402;
  public static final short NOT_FOUND_EXC = 404;
  public static final short ILLEGAL_METHOD_EXC = 405;
  public static final short REQUEST_TIMEOUT_EXC = 408;
  public static final short QUERY_LANGUAGE_UNKNOWN_EXC = 450;
  public static final short BAD_QUERY_EXC = 451;
  public static final short INVALID_PROPERTY_EXC = 452;
  public static final short INVALID_SESSIONID_EXC = 453;
  public static final short INVALID_SUBCOLLECTION_EXC = 454;
  public static final short MALFORMED_XML_EXC = 455;
  public static final short SERVER_ERROR_EXC = 500;
  public static final short NOT_IMPLEMENTED_EXC = 501;
  public static final short SERVICE_UNAVAILABLE_EXC = 503;

  short code;
  XMLObject details;

  public SDLIPException(short code, String description) {
    
    super(description);
    this.code = code;
  }

  public SDLIPException(short code, String description, XMLObject details) {

    this(code, description);
    this.details = details;
  }

  public short getCode() {
    return code;
  }

  public String getReason() {
    return getMessage();
  }

  public XMLObject getDetails() {
    return details;
  }
}
