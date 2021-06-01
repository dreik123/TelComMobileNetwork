#include <iostream>
#include <sysrepo.h>

using namespace std;

int main()
{
    int rc = SR_ERR_OK;
    sr_conn_ctx_t *connection = NULL;
    rc = sr_connect(0, &connection);

    if (rc != SR_ERR_OK){
        cout<< "not yet" << endl;
    }
    else {
        cout << "connected" << endl;
    }

    return 0;

}