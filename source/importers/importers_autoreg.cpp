#include <core/importer_factory.h>

#include "importers_autoreg.h"
#include "ImporterAVA.h"
#include "ImporterCHN.h"
#include "ImporterCNF.h"
#include "ImporterMCA.h"
#include "ImporterN42.h"
#include "ImporterSPC.h"
#include "ImporterTKA.h"


using namespace DAQuiri;

void importers_autoreg()
{
  DAQUIRI_REGISTER_IMPORTER(ImporterAVA)
  DAQUIRI_REGISTER_IMPORTER(ImporterCHN)
  DAQUIRI_REGISTER_IMPORTER(ImporterCNF)
  DAQUIRI_REGISTER_IMPORTER(ImporterMCA)
  DAQUIRI_REGISTER_IMPORTER(ImporterN42)
  DAQUIRI_REGISTER_IMPORTER(ImporterSPC)
  DAQUIRI_REGISTER_IMPORTER(ImporterTKA)
}