/**
 * @file io.h
 *
 * @date 24.08.2016
 * @author marco@kleesiek.com
 * @description
 */

#ifndef VMCMC_IO_H_
#define VMCMC_IO_H_

#include <vmcmc/sample.h>

#include <string>
#include <fstream>
#include <map>

namespace vmcmc
{

class Writer
{
public:
    Writer() { };
    virtual ~Writer() { };

    virtual Writer* Clone() const = 0;

    virtual void Write(size_t chainIndex, const Sample& sample) = 0;

protected:
    size_t fCurrentIndex = 0;
};

class TextFileWriter : public Writer
{
public:
    TextFileWriter(const std::string& directory = "", const std::string& stem = "vmcmc",
        const std::string& separator = "-", const std::string& extension = ".txt");
    virtual ~TextFileWriter();

    virtual TextFileWriter* Clone() const override;

    void SetPrecision(int prec) { fPrecision = prec; }
    void SetWriteCombined(bool combine) { fWriteCombined = combine; }

    std::string GetFilePath(int chainIndex = -1) const;

    virtual void Write(size_t chainIndex, const Sample& sample) override;

private:
    TextFileWriter(const TextFileWriter& other);
    void WriteFile(int streamIndex, const Sample& sample);

    std::string fDirectory;
    std::string fStem;
    std::string fSeparator;
    std::string fExtension;

    int fPrecision = 12;
    bool fWriteCombined = true;

    std::map<int, std::ofstream> fFileStreams;
};

} /* namespace vmcmc */

#endif /* VMCMC_IO_H_ */
