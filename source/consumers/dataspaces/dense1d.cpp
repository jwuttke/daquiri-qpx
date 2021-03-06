#include <consumers/dataspaces/dense1d.h>
#include <core/util/h5json.h>

namespace DAQuiri {

Dense1D::Dense1D()
    : Dataspace(1) {}

bool Dense1D::empty() const
{
  return spectrum_.empty();
}

void Dense1D::reserve(const Coords& limits)
{
  if (limits.size() != dimensions())
    return;
  spectrum_.resize(limits[0], PreciseFloat(0));
}

void Dense1D::clear()
{
  total_count_ = 0;
  maxchan_ = 0;
  spectrum_.clear();
}

void Dense1D::add(const Entry& e)
{
  if ((e.first.size() != dimensions()) || !e.second)
    return;
  const auto& bin = e.first[0];
  if (bin >= spectrum_.size())
    spectrum_.resize(bin + 1, PreciseFloat(0));

  spectrum_[bin] += e.second;
  total_count_ += e.second;
  maxchan_ = std::max(maxchan_, bin);
}

void Dense1D::add_one(const Coords& coords)
{
  if (coords.size() != dimensions())
    return;
  const auto& bin = coords[0];
  if (bin >= spectrum_.size())
    spectrum_.resize(bin + 1, PreciseFloat(0));

  spectrum_[bin]++;
  total_count_++;
  maxchan_ = std::max(maxchan_, bin);
}

void Dense1D::recalc_axes()
{
  auto ax = axis(0);
  ax.expand_domain(maxchan_);
  set_axis(0, ax);
}

PreciseFloat Dense1D::get(const Coords& coords) const
{
  if (coords.size() != dimensions())
    return 0;
  const auto& bin = *coords.begin();
  if (bin < spectrum_.size())
    return spectrum_.at(bin);
  return 0;
}

EntryList Dense1D::range(std::vector<Pair> list) const
{
  size_t min {0};
  size_t max {spectrum_.size() - 1};
  if (list.size() == dimensions())
  {
    min = std::max(list.begin()->first, (long unsigned) (0));
    max = std::min(list.begin()->second, spectrum_.size() - 1);
  }

  EntryList result(new EntryList_t);
  if (spectrum_.empty())
    return result;

  //TODO: only non-0s?
  for (size_t i = min; i <= max; ++i)
    result->push_back({{i}, spectrum_[i]});

  return result;
}

void Dense1D::data_save(const hdf5::node::Group& g) const
{
  if (!spectrum_.size())
    return;

  try
  {
    std::vector<double> d(maxchan_ + 1);
    for (uint32_t i = 0; i <= maxchan_; i++)
      d[i] = static_cast<double>(spectrum_[i]);

    auto dtype = hdf5::datatype::create<double>();
    auto dspace = hdf5::dataspace::Simple({d.size()});
    auto dset = g.create_dataset("counts", dtype, dspace);
    dset.write(d);
  }
  catch (...)
  {
    std::throw_with_nested(std::runtime_error("<Dense1D> Could not save"));
  }
}

void Dense1D::data_load(const hdf5::node::Group& g)
{
  try
  {
    std::vector<double> rdata;
    if (g.has_dataset("counts"))
    {
      auto dset = hdf5::node::Group(g).get_dataset("counts");
      auto shape = hdf5::dataspace::Simple(dset.dataspace()).current_dimensions();
      rdata.resize(shape[0]);
      dset.read(rdata);
    }

    if (spectrum_.size() != rdata.size())
    {
      spectrum_.clear();
      spectrum_.resize(rdata.size(), PreciseFloat(0));
    }

    maxchan_ = 0;
    total_count_ = 0;
    for (size_t i = 0; i < rdata.size(); i++)
    {
      total_count_ += rdata[i];
      spectrum_[i] = PreciseFloat(rdata[i]);
      if (rdata[i])
        maxchan_ = i;
    }
  }
  catch (...)
  {
    std::throw_with_nested(std::runtime_error("<Dense1D> Could not load"));
  }
}

std::string Dense1D::data_debug(const std::string& prepend) const
{
  std::stringstream ss;
  if (!spectrum_.size())
    return ss.str();

  PreciseFloat max = spectrum_[0];
  for (const auto& s : spectrum_)
    max = std::max(max, s);

  uint64_t nstars = 60;

  bool print {false};
  for (uint32_t i = 0; i <= maxchan_; i++)
  {
    double val = static_cast<double>(spectrum_[i]);
    if (val)
      print = true;
    if (print)
      ss << prepend << i << ": " <<
         std::string(nstars * val / max, '*') << "\n";
  }

  return ss.str();
}

void Dense1D::export_csv(std::ostream& os) const
{
  if (!spectrum_.size())
    return;

  for (uint32_t i = 0; i <= maxchan_; i++)
  {
    os << static_cast<double>(spectrum_[i]);
    if (i != maxchan_)
      os << ", ";
  }
}

}
