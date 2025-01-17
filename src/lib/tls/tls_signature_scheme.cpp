/*
* (C) 2022 Jack Lloyd
* (C) 2022 René Meusel, Hannes Rantzsch - neXenio GmbH
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#include <botan/tls_signature_scheme.h>

#include <botan/ec_group.h>
#include <botan/tls_exceptn.h>
#include <botan/tls_version.h>
#include <botan/internal/stl_util.h>

namespace Botan::TLS {

const std::vector<Signature_Scheme>& Signature_Scheme::all_available_schemes()
   {
   /*
   * This is ordered in some approximate order of preference
   */
   static const std::vector<Signature_Scheme> all_schemes = {

// EdDSA 25519 is currently not supported as a signature scheme for certificates
// certificate authentication.
// See: https://github.com/randombit/botan/pull/2958#discussion_r851294715
//
// #if defined(BOTAN_HAS_ED25519)
//       EDDSA_25519,
// #endif

      RSA_PSS_SHA384,
      RSA_PSS_SHA256,
      RSA_PSS_SHA512,

      RSA_PKCS1_SHA384,
      RSA_PKCS1_SHA512,
      RSA_PKCS1_SHA256,

      ECDSA_SHA384,
      ECDSA_SHA512,
      ECDSA_SHA256,
   };

   return all_schemes;
   }


Signature_Scheme::Signature_Scheme()
   : m_code(NONE)
   {}

Signature_Scheme::Signature_Scheme(uint16_t wire_code)
   : Signature_Scheme(Signature_Scheme::Code(wire_code))
   {}

Signature_Scheme::Signature_Scheme(Signature_Scheme::Code wire_code)
   : m_code(wire_code)
   {}

bool Signature_Scheme::is_available() const noexcept
   {
   return value_exists(Signature_Scheme::all_available_schemes(), *this);
   }

bool Signature_Scheme::is_set() const noexcept
   {
   return m_code != NONE;
   }

std::string Signature_Scheme::to_string() const noexcept
   {
   switch(m_code)
      {
      case RSA_PKCS1_SHA1:
         return "RSA_PKCS1_SHA1";
      case RSA_PKCS1_SHA256:
         return "RSA_PKCS1_SHA256";
      case RSA_PKCS1_SHA384:
         return "RSA_PKCS1_SHA384";
      case RSA_PKCS1_SHA512:
         return "RSA_PKCS1_SHA512";

      case ECDSA_SHA1:
         return "ECDSA_SHA1";
      case ECDSA_SHA256:
         return "ECDSA_SHA256";
      case ECDSA_SHA384:
         return "ECDSA_SHA384";
      case ECDSA_SHA512:
         return "ECDSA_SHA512";

      case RSA_PSS_SHA256:
         return "RSA_PSS_SHA256";
      case RSA_PSS_SHA384:
         return "RSA_PSS_SHA384";
      case RSA_PSS_SHA512:
         return "RSA_PSS_SHA512";

      case EDDSA_25519:
         return "EDDSA_25519";
      case EDDSA_448:
         return "EDDSA_448";

      case DSA_SHA1:
         return "DSA_SHA1";
      case DSA_SHA256:
         return "DSA_SHA256";
      case DSA_SHA384:
         return "DSA_SHA384";
      case DSA_SHA512:
         return "DSA_SHA512";

      default:
         return "Unknown signature scheme: " + std::to_string(m_code);
      }
   }

std::string Signature_Scheme::hash_function_name() const noexcept
   {
   switch(m_code)
      {
      case RSA_PKCS1_SHA1:
      case ECDSA_SHA1:
      case DSA_SHA1:
         return "SHA-1";

      case ECDSA_SHA256:
      case RSA_PKCS1_SHA256:
      case RSA_PSS_SHA256:
      case DSA_SHA256:
         return "SHA-256";

      case ECDSA_SHA384:
      case RSA_PKCS1_SHA384:
      case RSA_PSS_SHA384:
      case DSA_SHA384:
         return "SHA-384";

      case ECDSA_SHA512:
      case RSA_PKCS1_SHA512:
      case RSA_PSS_SHA512:
      case DSA_SHA512:
         return "SHA-512";

      case EDDSA_25519:
      case EDDSA_448:
         return "Pure";

      default:
         return "Unknown hash function";
      }
   }

std::string Signature_Scheme::padding_string() const noexcept
   {
   switch(m_code)
      {
      case RSA_PKCS1_SHA1:
         return "EMSA_PKCS1(SHA-1)";
      case RSA_PKCS1_SHA256:
         return "EMSA_PKCS1(SHA-256)";
      case RSA_PKCS1_SHA384:
         return "EMSA_PKCS1(SHA-384)";
      case RSA_PKCS1_SHA512:
         return "EMSA_PKCS1(SHA-512)";

      case ECDSA_SHA1:
         return "EMSA1(SHA-1)";
      case ECDSA_SHA256:
         return "EMSA1(SHA-256)";
      case ECDSA_SHA384:
         return "EMSA1(SHA-384)";
      case ECDSA_SHA512:
         return "EMSA1(SHA-512)";

      case RSA_PSS_SHA256:
         return "PSSR(SHA-256,MGF1,32)";
      case RSA_PSS_SHA384:
         return "PSSR(SHA-384,MGF1,48)";
      case RSA_PSS_SHA512:
         return "PSSR(SHA-512,MGF1,64)";

      case EDDSA_25519:
         return "Pure";
      case EDDSA_448:
         return "Pure";

      default:
         return "Unknown padding";
      }
   }

std::string Signature_Scheme::algorithm_name() const noexcept
   {
   switch(m_code)
      {
      case RSA_PKCS1_SHA1:
      case RSA_PKCS1_SHA256:
      case RSA_PKCS1_SHA384:
      case RSA_PKCS1_SHA512:
      case RSA_PSS_SHA256:
      case RSA_PSS_SHA384:
      case RSA_PSS_SHA512:
         return "RSA";

      case ECDSA_SHA1:
      case ECDSA_SHA256:
      case ECDSA_SHA384:
      case ECDSA_SHA512:
         return "ECDSA";

      case EDDSA_25519:
         return "Ed25519";

      case EDDSA_448:
         return "Ed448";

      case DSA_SHA1:
      case DSA_SHA256:
      case DSA_SHA384:
      case DSA_SHA512:
         return "DSA";

      default:
         return "Unknown algorithm";
      }
   }

AlgorithmIdentifier Signature_Scheme::algorithm_identifier() const noexcept
   {
   switch(m_code)
      {
      // case ECDSA_SHA1:  not defined
      case ECDSA_SHA256:
         return { "ECDSA", Botan::EC_Group("secp256r1").DER_encode(Botan::EC_Group_Encoding::NamedCurve) };
      case ECDSA_SHA384:
         return { "ECDSA", Botan::EC_Group("secp384r1").DER_encode(Botan::EC_Group_Encoding::NamedCurve) };
      case ECDSA_SHA512:
         return { "ECDSA", Botan::EC_Group("secp521r1").DER_encode(Botan::EC_Group_Encoding::NamedCurve) };

      case EDDSA_25519:
         return { "Ed25519", AlgorithmIdentifier::USE_EMPTY_PARAM };

      case RSA_PKCS1_SHA1:
      case RSA_PKCS1_SHA256:
      case RSA_PKCS1_SHA384:
      case RSA_PKCS1_SHA512:
      case RSA_PSS_SHA256:
      case RSA_PSS_SHA384:
      case RSA_PSS_SHA512:
         return { "RSA", AlgorithmIdentifier::USE_NULL_PARAM };

      default:
         return AlgorithmIdentifier();
      }
   }

std::optional<Signature_Format> Signature_Scheme::format() const noexcept
   {
   switch(m_code)
      {
      case RSA_PKCS1_SHA1:
      case RSA_PKCS1_SHA256:
      case RSA_PKCS1_SHA384:
      case RSA_PKCS1_SHA512:
      case RSA_PSS_SHA256:
      case RSA_PSS_SHA384:
      case RSA_PSS_SHA512:
         return IEEE_1363;

      case ECDSA_SHA1:
      case ECDSA_SHA256:
      case ECDSA_SHA384:
      case ECDSA_SHA512:
      case EDDSA_25519:
      case EDDSA_448:
      case DSA_SHA1:
      case DSA_SHA256:
      case DSA_SHA384:
      case DSA_SHA512:
         return DER_SEQUENCE;

      default:
         return std::nullopt;
      }
   }

bool Signature_Scheme::is_compatible_with(const Protocol_Version& protocol_version) const noexcept
   {
   // RFC 8446 4.4.3:
   //   The SHA-1 algorithm MUST NOT be used in any signatures of
   //   CertificateVerify messages.
   //
   // Note that Botan enforces that for TLS 1.2 as well.
   if(hash_function_name() == "SHA-1")
      return false;

   // RFC 8446 4.4.3:
   //   RSA signatures MUST use an RSASSA-PSS algorithm, regardless of whether
   //   RSASSA-PKCS1-v1_5 algorithms appear in "signature_algorithms".
   //
   // Note that this is enforced for TLS 1.3 and above only.
   if(!protocol_version.is_pre_tls_13() &&
       (m_code == RSA_PKCS1_SHA1   ||
        m_code == RSA_PKCS1_SHA256 ||
        m_code == RSA_PKCS1_SHA384 ||
        m_code == RSA_PKCS1_SHA512))
      return false;

   return true;
   }

bool Signature_Scheme::is_suitable_for(const Private_Key &private_key) const noexcept
   {
   if(algorithm_name() != private_key.algo_name())
      return false;

   // The ECDSA private key length must match the utilized hash output length.
   const auto keylen = private_key.key_length();
   if(keylen <= 250)
      return false;

   if(m_code == ECDSA_SHA256 && !(keylen >= 250 && keylen <= 350)) // lgtm [cpp/constant-comparison]
      return false;                                                // `keylen >= 250` will always be true, because keylen <= 250
                                                                   // was checked before. Leaving it in for readability.
   if(m_code == ECDSA_SHA384 && !(keylen >= 350 && keylen <= 450))
      return false;

   if(m_code == ECDSA_SHA512 && !(keylen >= 450 && keylen <= 550))
      return false;

   return true;
   }

}  // Botan::TLS
