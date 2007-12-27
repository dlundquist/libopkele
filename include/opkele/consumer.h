#ifndef __OPKELE_CONSUMER_H
#define __OPKELE_CONSUMER_H

#include <opkele/types.h>
#include <opkele/extension.h>

/**
 * @file
 * @brief OpenID consumer-side functionality
 */

namespace opkele {

    /**
     * implementation of basic consumer functionality
     *
     * @note
     * The consumer uses libcurl internally, which means that if you're using
     * libopkele in multithreaded environment you should call curl_global_init
     * yourself before spawning any threads.
     */
    class consumer_t {
	public:

	    virtual ~consumer_t() { }

	    /**
	     * store association. The function should be overridden in the real
	     * implementation to provide persistent associations store.
	     * @param server the OpenID server
	     * @param handle association handle
	     * @param secret the secret associated with the server and handle
	     * @param expires_in the number of seconds until the handle is expired
	     * @return the assoc_t for the newly allocated association_t object
	     */
	    virtual assoc_t store_assoc(const string& server,const string& handle,const secret_t& secret,int expires_in) = 0;
	    /**
	     * retrieve stored association. The function should be overridden
	     * in the real implementation to provide persistent assocations
	     * store.
	     *
	     * @note
	     * The user is responsible for handling associations expiry and
	     * this function should never return an expired or invalidated
	     * association.
	     *
	     * @param server the OpenID server
	     * @param handle association handle
	     * @return the autho_ptr<> for the newly allocated association_t object
	     * @throw failed_lookup if no unexpired association found
	     */
	    virtual assoc_t retrieve_assoc(const string& server,const string& handle) = 0;
	    /**
	     * invalidate stored association. The function should be overridden
	     * in the real implementation of the consumer.
	     * @param server the OpenID server
	     * @param handle association handle
	     */
	    virtual void invalidate_assoc(const string& server,const string& handle) = 0;
	    /**
	     * retrieve any unexpired association for the server. If the
	     * function is not overridden in the real implementation, the new
	     * association will be established for each request.
	     *
	     * @note
	     * The user is responsible for handling associations and this
	     * function should never return an expired or invalidated
	     * association.
	     *
	     * @note
	     * It may be a good idea to pre-expire associations shortly before
	     * their time is really up to avoid association expiry in the
	     * middle of negotiations.
	     *
	     * @param server the OpenID server
	     * @return the assoc_t for the newly allocated association_t object
	     * @throw failed_lookup in case of absence of the handle
	     */
	    virtual assoc_t find_assoc(const string& server);

	    /**
	     * retrieve the metainformation contained in link tags from the
	     * page pointed by url. the function may implement caching of the
	     * information.
	     * @param url url to harvest for link tags
	     * @param server reference to the string object where to put
	     * openid.server value
	     * @param delegate reference to the string object where to put the
	     * openid.delegate value (if any)
	     */
	    virtual void retrieve_links(const string& url,string& server,string& delegate);

	    /**
	     * perform the associate request to OpenID server.
	     * @param server the OpenID server
	     * @return the assoc_t for the newly allocated association_t
	     * object, representing established association
	     * @throw exception in case of error
	     */
	    assoc_t associate(const string& server);
	    /**
	     * prepare the parameters for the checkid_immediate
	     * request.
	     * @param identity the identity to verify
	     * @param return_to the return_to url to pass with the request
	     * @param trust_root the trust root to advertise with the request
	     * @param ext pointer to an extension(s) hooks object
	     * @return the location string
	     * @throw exception in case of error
	     */
	    virtual string checkid_immediate(const string& identity,const string& return_to,const string& trust_root="",extension_t *ext=0);
	    /**
	     * prepare the parameters for the checkid_setup
	     * request.
	     * @param identity the identity to verify
	     * @param return_to the return_to url to pass with the request
	     * @param trust_root the trust root to advertise with the request
	     * @param ext pointer to an extension(s) hooks object
	     * @return the location string
	     * @throw exception in case of error
	     */
	    virtual string checkid_setup(const string& identity,const string& return_to,const string& trust_root="",extension_t *ext=0);
	    /**
	     * the actual implementation behind checkid_immediate() and
	     * checkid_setup() functions.
	     * @param mode checkid_* mode - either mode_checkid_immediate or mode_checkid_setup
	     * @param identity the identity to verify
	     * @param return_to the return_to url to pass with the request
	     * @param trust_root the trust root to advertise with the request
	     * @param ext pointer to an extension(s) hooks object
	     * @return the location string
	     * @throw exception in case of error
	     */
	    virtual string checkid_(mode_t mode,const string& identity,const string& return_to,const string& trust_root="",extension_t *ext=0);
	    /**
	     * verify the id_res response
	     * @param pin the response parameters
	     * @param identity the identity being checked (if not specified,
	     * @param ext pointer to an extension(s) hooks object
	     * extracted from the openid.identity parameter
	     * @throw id_res_mismatch in case of signature mismatch
	     * @throw id_res_setup in case of openid.user_setup_url failure
	     * (supposedly checkid_immediate only)
	     * @throw id_res_failed in case of failure
	     * @throw id_res_expired_on_delivery if the association expired before it could've been verified
	     * @throw exception in case of other failures
	     */
	    virtual void id_res(const params_t& pin,const string& identity="",extension_t *ext=0);
	    /**
	     * perform a check_authentication request.
	     * @param server the OpenID server
	     * @param p request parameters
	     */
	    void check_authentication(const string& server,const params_t& p);

	    /**
	     * normalize URL by adding http:// and trailing slash if needed.
	     * @param url
	     * @return normalized url
	     */
	    static string normalize(const string& url);

	    /**
	     * Canonicalize URL, by normalizing its appearance and following redirects.
	     * @param url
	     * @return canonicalized url
	     */
	    virtual string canonicalize(const string& url);

    };

}

#endif /* __OPKELE_CONSUMER_H */
