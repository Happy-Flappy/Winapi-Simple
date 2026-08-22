RWByteAddressBuffer Output : register(u0);
ByteAddressBuffer Input : register(t0);

[numthreads(64, 1, 1)]
void CSMain(uint3 id : SV_DispatchThreadID)
{
    uint index = id.x;
    // Read the float at offset 'index * 4'
    float val = asfloat(Input.Load(index * 4));
    // Write doubled value back
    Output.Store(index * 4, asuint(val * 2.0f));
}